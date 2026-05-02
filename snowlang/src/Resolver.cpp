#include "Resolver.hpp"
#include "AST.hpp"
#include "Location.hpp"
#include "ResolvedAST.hpp"
#include "SnowErr.hpp"
#include "SnowlangHelper.hpp"
#include "SnowlangInstance.hpp"
#include "Tokenizer.hpp"

#include <array>
#include <cstddef>
#include <string>
#include <unordered_map>

namespace Snowlang {

Resolver::Resolver(SnowlangInstance *owner) : owner(owner) {}

bool Resolver::isAssignable(const RExprPtr &expr) {
  switch (expr->kind) {

  case ExprKind::VarRef:
  case ExprKind::This:
    return true;

  case ExprKind::MemberAccess: {
    auto *m = static_cast<RMemberAccessExpr *>(expr.get());
    return isAssignable(m->base);
  }

  case ExprKind::Indexing: {
    auto *i = static_cast<RIndexExpr *>(expr.get());
    return isAssignable(i->base);
  }

  default:
    return false;
  }
}

void Resolver::dumpLambdaStack(const std::vector<LambdaScope> &stack, const std::string &var) {
  owner->debug.logln("---- Lambda stack dump for variable '", var, "' ----");
  for (size_t i = 0; i < stack.size(); ++i) {
    owner->debug.log("Lambda[", i, "] captures: ");
    if (stack[i].captures.empty()) {
      owner->debug.log("<empty>");
    } else {
      for (auto &[name, cap] : stack[i].captures) {
        owner->debug.log(name, "(slot ", cap.captureSlot, ") ");
      }
    }
    owner->debug.logln();
  }
  owner->debug.logln("-----------------------------------------------");
}

void Resolver::enterLocalScope() {
  size_t initialSlotCount = localSlotCounters.empty() ? 0 : localSlotCounters.back();

  localSymbolsStack.emplace_back();
  localSlotCounters.push_back(initialSlotCount);
}

void Resolver::leaveLocalScope() {
  if (localSymbolsStack.empty()) {
    throwError(SnowErr::Phase::Resolver, "internal resolver error: leaving empty local scope",
               SourceSpan{.start = 0, .end = 0, .line = 0, .column = 0});
  }

  localSymbolsStack.pop_back();
  localSlotCounters.pop_back();
}

size_t Resolver::getNewCommandId(const std::string &name) {
  auto it = commandIds.find(name);
  if (it != commandIds.end())
    return it->second;

  size_t id = nextCommandId++;
  commandIds[name] = id;

  owner->debug.logln("[Command] ", name, " -> id ", id);
  return id;
}

Location Resolver::resolveVarCreation(const std::string &name, const SourceSpan &span) {
  (void)span;

  if (localSymbolsStack.empty()) {
    size_t slot = nextGlobalSlot++;
    globalSymbols[name] = slot;
    return {Location::Type::Global, slot};
  }

  size_t slot = localSlotCounters.back()++;
  localSymbolsStack.back()[name] = slot;
  return {Location::Type::Local, slot};
}

Location Resolver::resolveCall(const std::string &name, const SourceSpan &currentExprSpan) {
  owner->debug.logln("\n[resolveCall] resolving '", name, "'");
  owner->debug.logln("localSymbolsStack size = ", localSymbolsStack.size());
  owner->debug.logln("lambdaStack size       = ", lambdaStack.size());

  for (size_t stackPos = localSymbolsStack.size(); stackPos-- > 0;) {
    auto &scope = localSymbolsStack[stackPos];
    auto it = scope.find(name);
    if (it == scope.end())
      continue;

    owner->debug.logln("Found '", name, "' in scope index ", stackPos, " (current scope index ",
                       localSymbolsStack.size() - 1, ")");

    if (stackPos == localSymbolsStack.size() - 1) {
      return {Location::Type::Local, it->second};
    }

    //----CAPTURE CHAIN----
    size_t definingScope = stackPos;

    Location source(Location::Type::Local, it->second);
    for (size_t i = definingScope + 1; i < lambdaStack.size(); ++i) {
      auto &lambda = lambdaStack[i];
      auto capIt = lambda.captures.find(name);

      if (capIt == lambda.captures.end()) {
        size_t slot = lambda.nextCaptureSlot++;

        owner->debug.logln(">>>>Captured: ", Debug::locationToString(source));
        lambda.captures.emplace(name, CaptureInfo{slot, source});

        owner->debug.logln("Created capture '", name, "' in lambda ", i, " slot ", slot,
                           " source=", Debug::locationToString(source));

        source = {Location::Type::Capture, slot};
      } else {
        source = {Location::Type::Capture, capIt->second.captureSlot};
      }
    }

    return source;
  }

  auto it = globalSymbols.find(name);
  if (it == globalSymbols.end()) {
    throwError(SnowErr::Phase::Resolver, "use of undefined variable '" + name + "'",
               currentExprSpan, "this name is not defined in any accessible scope");
  }

  return {Location::Type::Global, it->second};
}

void Resolver::resolvePattern(const PatternPtr &pattern, PatternBindMode mode,
                              std::vector<size_t> &outSlots) {
  if (auto list = dynamic_cast<IdentListPattern *>(pattern.get())) {
    for (const auto &name : list->names) {
      Location location = resolveVarCreation(name, pattern->span);

      owner->debug.logln("[Resolve Pattern][Var Creation] ", name, " | ",
                         Debug::locationToString(location));

      if (mode == PatternBindMode::LambdaParam && location.type == Location::Type::Global) {
        throwError(SnowErr::Phase::Resolver, "invalid lambda parameter binding", pattern->span,
                   "lambda parameters must be local variables");
      }

      outSlots.push_back(location.slot);
    }
    return;
  }

  throwError(SnowErr::Phase::Resolver, "unsupported pattern in binding", pattern->span);
}

RStmtPtr Resolver::resolveStmt(const StmtPtr &stmt) {
  if (auto exprStmt = dynamic_cast<ExprStmt *>(stmt.get())) {
    return std::make_shared<RExprStmt>(resolveExpr(exprStmt->expr));
  }

  //----------- let ----------
  if (auto l = dynamic_cast<LetStmt *>(stmt.get())) {
    Location loc = resolveVarCreation(l->name, stmt->span);

    RExprPtr assignExpr = nullptr;
    if (l->assignment)
      assignExpr = resolveExpr(l->assignment);

    owner->debug.logln("[Let] ", l->name, " | ", Debug::locationToString(loc));

    return std::make_shared<RLetStmt>(loc, assignExpr);
  }

  //----------- return  ----------
  if (auto ret = dynamic_cast<ReturnStmt *>(stmt.get())) {
    if (lambdaStack.empty()) {
      throwError(SnowErr::Phase::Resolver, "'return' used outside of a lambda", ret->span);
    }

    return std::make_shared<RReturnStmt>(resolveExpr(ret->toReturn));
  }

  //----------- break ----------
  if (dynamic_cast<BreakStmt *>(stmt.get())) {
    return std::make_shared<RBreakStmt>();
  }

  //----------- continue ----------
  if (dynamic_cast<ContinueStmt *>(stmt.get())) {
    return std::make_shared<RContinueStmt>();
  }

  //----------- import -----------
  if (auto imp = dynamic_cast<ImportStmt *>(stmt.get())) {
    return std::make_shared<RImportStmt>(resolveExpr(imp->path));
  }

  //----------- if ----------
  if (auto ifstmt = dynamic_cast<IfStmt *>(stmt.get())) {
    RExprPtr condition = resolveExpr(ifstmt->condition);
    RStmtPtr thenBranch = resolveStmt(ifstmt->thenBranch);

    RStmtPtr elseBranch = nullptr;
    if (ifstmt->elseBranch) {
      elseBranch = resolveStmt(ifstmt->elseBranch);
    }

    return std::make_shared<RIfStmt>(condition, thenBranch, elseBranch);
  }

  //----------- while ----------
  if (auto ifstmt = dynamic_cast<WhileStmt *>(stmt.get())) {
    RExprPtr condition = resolveExpr(ifstmt->condition);
    RStmtPtr doBranch = resolveStmt(ifstmt->doBranch);

    return std::make_shared<RWhileStmt>(condition, doBranch);
  }

  //----------- do ----------
  if (auto dostmt = dynamic_cast<DoStmt *>(stmt.get())) {
    std::vector<RStmtPtr> body;
    body.reserve(dostmt->body.size());
    for (auto &s : dostmt->body) {
      body.push_back(resolveStmt(s));
    }
    return std::make_shared<RDoStmt>(std::move(body));
  }

  throwError(SnowErr::Phase::Resolver, "unsupported statement", stmt->span);
}

RExprPtr Resolver::resolveExpr(const ExprPtr &expr) {
  if (auto n = dynamic_cast<NumberExpr *>(expr.get()))
    return std::make_shared<RNumberExpr>(n->value);

  if (auto s = dynamic_cast<StringExpr *>(expr.get()))
    return std::make_shared<RStringExpr>(s->text);

  //-----Load $ -----
  if (auto l = dynamic_cast<LoadExpr *>(expr.get())) {
    Location location = resolveCall(l->name, expr->span);

    owner->debug.logln("[$Load] ", l->name, " | ", Debug::locationToString(location));

    return std::make_shared<RVarRefExpr>(location);
  }

  //-----CellRef @ -----
  if (auto l = dynamic_cast<AddressExpr *>(expr.get())) {
    Location location = resolveCall(l->name, expr->span);

    owner->debug.logln("[@CellRef] ", l->name, " | ", Debug::locationToString(location));

    return std::make_shared<RAddressExpr>(location);
  }

  //----Assignment = ----
  if (auto asg = dynamic_cast<AssignmentExpr *>(expr.get())) {
    RExprPtr lhs = resolveExpr(asg->lhs);
    RExprPtr rhs = resolveExpr(asg->rhs);

    if (!isAssignable(lhs)) {
      throwError(SnowErr::Phase::Resolver, "invalid assignment target", asg->lhs->span,
                 "left-hand side is not assignable");
    }

    return std::make_shared<RAssignExpr>(lhs, rhs);
  }

  //--- Indexing [] ---
  if (auto idx = dynamic_cast<IndexExpr *>(expr.get())) {
    return std::make_shared<RIndexExpr>(resolveExpr(idx->base), resolveExpr(idx->index));
  }

  //----Lambdas----
  if (auto l = dynamic_cast<LambdaExpr *>(expr.get())) {
    lambdaStack.push_back(LambdaScope{});
    enterLocalScope();

    // change it so resolvePattern returns a std::vector<size_t> instead of having to pass the ref;
    std::vector<size_t> paramSlots;
    resolvePattern(l->params, PatternBindMode::LambdaParam, paramSlots);

    std::vector<RStmtPtr> body;
    body.reserve(l->body.size());
    for (auto &stmt : l->body) {
      body.push_back(resolveStmt(stmt));
    }

    size_t frameSize = localSlotCounters.back();
    leaveLocalScope();

    auto lambdaScope = std::move(lambdaStack.back());
    lambdaStack.pop_back();

    std::vector<CaptureDesc> captureVec(lambdaScope.nextCaptureSlot);

    for (auto &[_, cap] : lambdaScope.captures) {
      captureVec[cap.captureSlot] = CaptureDesc{cap.sourceLocation};
    }

    owner->debug.logln("[Resolver] Lambda Body Size: ", body.size());

    return std::make_shared<RLambdaExpr>(nextLambdaId++, paramSlots, body, frameSize, captureVec);
  }

  //---Lambda Calls---
  if (auto call = dynamic_cast<CallExpr *>(expr.get())) {
    auto callee = resolveExpr(call->callee);

    std::vector<RExprPtr> args;
    args.reserve(call->args.size());
    for (auto &arg : call->args) {
      args.push_back(resolveExpr(arg));
    }

    return std::make_shared<RCallExpr>(callee, args);
  }

  //----commands----
  if (auto cmd = dynamic_cast<CommandExpr *>(expr.get())) {
    std::vector<RExprPtr> args;
    args.reserve(cmd->args.size());
    for (auto &arg : cmd->args) {
      args.push_back(resolveExpr(arg));
    }

    std::unordered_map<std::string, RExprPtr> resolvedFlags;
    for (auto &[name, flag] : cmd->flags) {
      resolvedFlags.emplace(name, resolveExpr(flag));
    }

    return std::make_shared<RCommandExpr>(getNewCommandId(cmd->command), std::move(args),
                                          std::move(resolvedFlags));
  }

  //----Parameter----
  if (auto p = dynamic_cast<ParameterExpr *>(expr.get())) {
    return std::make_shared<RStringExpr>(p->text);
  }

  //---Binary---
  if (auto b = dynamic_cast<BinaryExpr *>(expr.get())) {
    auto left = resolveExpr(b->left);
    auto right = resolveExpr(b->right);

    return std::make_shared<RBinaryExpr>(b->op, std::move(left), std::move(right));
  }

  //------Object----
  if (auto o = dynamic_cast<ObjectExpr *>(expr.get())) {
    std::vector<RObjectExpr::Field> fields;
    fields.reserve(o->fields.size());
    for (auto &f : o->fields) {
      fields.push_back({f.name, resolveExpr(f.value)});
    }

    return std::make_shared<RObjectExpr>(std::move(fields), expr->span);
  }

  //----Member----
  if (auto m = dynamic_cast<MemberAccessExpr *>(expr.get())) {
    RExprPtr base = resolveExpr(m->base);

    auto baseLoc = SnowlangHelper::extractAssignableBase(base);

    return std::make_shared<RMemberAccessExpr>(base, m->member, baseLoc, expr->span);
  }

  //-----This-----
  if (dynamic_cast<ThisExpr *>(expr.get())) {
    return std::make_shared<RThisExpr>();
  }

  //-----Bool----
  if (auto b = dynamic_cast<BoolExpr *>(expr.get())) {
    return std::make_shared<RBoolExpr>(b->val);
  }

  //-----NULL----
  if (dynamic_cast<NullExpr *>(expr.get())) {
    return std::make_shared<RNullExpr>();
  }

  throwError(SnowErr::Phase::Resolver, "unsupported expression", expr->span);
}

const std::unordered_map<std::string, size_t> &Resolver::getGlobalSymbols() const {
  return globalSymbols;
}

} // namespace Snowlang