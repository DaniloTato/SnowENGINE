#include "Evaluator.hpp"
#include "CommandSignature.hpp"
#include "Commands.hpp"
#include "LValue.hpp"
#include "LambdaInstance.hpp"
#include "Location.hpp"
#include "ObjectInstance.hpp"
#include "ResolvedAST.hpp"
#include "RuntimeValue.hpp"
#include "SnowErr.hpp"
#include "SnowlangHelper.hpp"
#include "SnowlangInstance.hpp"
#include "Tokenizer.hpp"

#include <memory>
#include <string>
#include <variant>

namespace Snowlang {

Evaluator::Evaluator(SnowlangInstance *owner) : owner(owner) {}

CommandSignature Evaluator::tryGetCommandId(size_t slot, const SourceSpan &span) {
  try {
    return owner->memory.getCommandById(slot);
  } catch (const SnowErr::MemoryErr &e) {
    throwError(SnowErr::Phase::Memory, "failed fetching command", span, e.what());
  }
}

void Evaluator::tryPopCallFrame(const SourceSpan &span) {
  try {
    owner->memory.popCallFrame();
  } catch (const SnowErr::MemoryErr &e) {
    throwError(SnowErr::Phase::Memory, "popped empty call frame", span, e.what());
  }
}

RuntimeValue Evaluator::tryMemoryRead(const Location &loc, const SourceSpan &span) {
  try {
    return owner->memory.read(loc);
  } catch (const SnowErr::MemoryErr &e) {
    throwError(SnowErr::Phase::Memory, "invalid variable access", span, e.what());
  }
}

CellPtr Evaluator::tryMemoryCellRead(const Location &loc, const SourceSpan &span) {
  try {
    return owner->memory.readCell(loc);
  } catch (const SnowErr::MemoryErr &e) {
    throwError(SnowErr::Phase::Memory, "invalid cell read", span, e.what());
  }
}

void Evaluator::tryMemoryWrite(const Location &loc, RuntimeValue &value, const SourceSpan &span) {
  try {
    owner->memory.write(loc, value);
  } catch (const SnowErr::MemoryErr &e) {
    throwError(SnowErr::Phase::Memory, "invalid variable access", span, e.what());
  }
}

LValue Evaluator::evalLValue(const RExprPtr &expr) {
  switch (expr->kind) {

  case ExprKind::VarRef: {
    auto *v = static_cast<RVarRefExpr *>(expr.get());
    return LValue{.kind = LValue::Kind::Variable, .location = v->location};
  }

  case ExprKind::MemberAccess: {
    auto *m = static_cast<RMemberAccessExpr *>(expr.get());
    RuntimeValue base = evalExpr(m->base);

    if (!std::holds_alternative<ObjectRef>(base.data)) {
      throwError(SnowErr::Phase::Evaluator, "assignment to non-object member", m->span);
    }

    return LValue{
        .kind = LValue::Kind::ObjectField,
        .object = std::get<ObjectRef>(base.data),
        .field = m->member,
    };
  }

  case ExprKind::Indexing: {
    auto *i = static_cast<RIndexExpr *>(expr.get());

    LValue base = evalLValue(i->base);

    RuntimeValue container = base.LValueRead(*owner, i->span);

    if (!std::holds_alternative<RuntimeValue::List>(container.data)) {
      throwError(SnowErr::Phase::Evaluator, "indexing non-list", i->span);
    }

    int idx = static_cast<int>(std::get<float>(evalExpr(i->index).data));

    const auto &list = std::get<RuntimeValue::List>(container.data);

    if (idx < 0 || idx >= (int)list.size()) {
      throwError(SnowErr::Phase::Evaluator, "index out of bounds", i->span);
    }

    return LValue{.kind = LValue::Kind::ListElement,
                  .base = std::make_unique<LValue>(std::move(base)),
                  .index = idx};
  }

  default: {
    throwError(SnowErr::Phase::Evaluator, "Invalid left hand side of assignment", expr->span);
  }
  }
}

RuntimeValue Evaluator::evalExpr(const RExprPtr &expr) {
  switch (expr->kind) {
  case ExprKind::Number: {
    auto *n = static_cast<RNumberExpr *>(expr.get());
    return {n->value};
  }

  case ExprKind::String: {
    auto *s = static_cast<RStringExpr *>(expr.get());
    return {s->value};
  }

  // ---------- variable reference ----------
  case ExprKind::VarRef: {
    auto *v = static_cast<RVarRefExpr *>(expr.get());
    return tryMemoryRead(v->location, v->span);
  }

  // ---------- CellRef @ ----------
  case ExprKind::Address: {
    auto *cellRef = static_cast<RAddressExpr *>(expr.get());
    CellPtr ptr = tryMemoryCellRead(cellRef->location, cellRef->span);
    return {ptr};
  }

  // ---------- assignment = ----------
  case ExprKind::Assign: {
    auto *a = static_cast<RAssignExpr *>(expr.get());

    RuntimeValue value = evalExpr(a->rhs);
    LValue target = evalLValue(a->lhs);

    target.LValueWrite(value, *owner);
    return value;
  }

  // ----------- indexing [] -------
  case ExprKind::Indexing: {
    auto *indexing = static_cast<RIndexExpr *>(expr.get());
    RuntimeValueRef base = evalExpr(indexing->base);
    RuntimeValueRef index = evalExpr(indexing->index);

    if (!std::holds_alternative<float>(index.data)) {
      throwError(SnowErr::Phase::Evaluator, "index must be a number", indexing->index->span);
    }

    int i = static_cast<int>(std::get<float>(index.data));

    // LIST INDEXING
    if (std::holds_alternative<RuntimeValue::List>(base.data)) {
      const auto &list = std::get<RuntimeValue::List>(base.data);

      if (i < 0 || i >= (int)list.size()) {
        throwError(SnowErr::Phase::Evaluator, "index out of bounds", indexing->index->span);
      }

      RuntimeValue val = list[i];
      if (std::holds_alternative<RuntimeValue::GameObjectRef>(val.data)) {
        return std::get<RuntimeValue::GameObjectRef>(val.data).getter();
      }

      return val;
    }

    // STRING INDEXING
    if (std::holds_alternative<std::string>(base.data)) {
      const auto &str = std::get<std::string>(base.data);

      if (i < 0 || i >= (int)str.size()) {
        throwError(SnowErr::Phase::Evaluator, "index out of bounds", indexing->index->span);
      }

      std::string result(1, str[i]);
      return {result};
    }

    throwError(SnowErr::Phase::Evaluator, "only list and string values can be indexed",
               indexing->base->span);
  }

  // ----------- commands -------
  case ExprKind::Command: {
    auto *cmd = static_cast<RCommandExpr *>(expr.get());
    return evalCommand(*cmd, cmd->span);
  }

  // ----------- lambdas -------
  case ExprKind::Lambda: {
    auto *l = static_cast<RLambdaExpr *>(expr.get());

    owner->debug.logln("lambda id=", l->id, " captures declared=", l->captures.size());

    owner->debug.logln("[Evaluator] Creating lambda instance id=", std::to_string(l->id));
    owner->debug.logln("[Evaluator] Lambda AST node body size: ", l->body.size());

    std::vector<CellPtr> evalCaptures;
    for (size_t i = 0; i < l->captures.size(); ++i) {
      const CaptureDesc &cap = l->captures[i];

      owner->debug.logln("  [Capture] slot ", i, " <- ", Debug::locationToString(cap.source));

      CellPtr capturedCell = tryMemoryCellRead(cap.source, l->span);
      evalCaptures.push_back(capturedCell);
    }

    owner->debug.logln("[Evaluator] Lambda ready (captures=", evalCaptures.size(), ")");
    owner->debug.logln("[Evaluator] Lambda Instance body size: ", evalCaptures.size());

    return {std::make_shared<LambdaInstance>(
        LambdaInstance{l->id, l->paramSlots, l->body, l->frameSize, evalCaptures})};
  }

  //----Call-----
  case ExprKind::Call: {
    auto *call = static_cast<RCallExpr *>(expr.get());

    owner->debug.logln("[Evaluator] Call expression");

    ObjectRef thisObject = nullptr;
    const RuntimeValue *calleeValue;
    RuntimeValue tempCallee;

    if (call->callee->kind == ExprKind::MemberAccess) {
      auto *m = static_cast<RMemberAccessExpr *>(call->callee.get());
      RuntimeValueRef base = evalExpr(m->base);
      if (!std::holds_alternative<ObjectRef>(base.data)) {
        throwError(SnowErr::Phase::Evaluator, "method call on non-object", m->span);
      }
      thisObject = std::get<ObjectRef>(base.data);
      auto it = thisObject->fields.find(m->member);
      if (it == thisObject->fields.end()) {
        throwError(SnowErr::Phase::Evaluator, "object has no method '" + m->member + "'", m->span);
      }
      calleeValue = &it->second;
    } else {
      tempCallee = evalExpr(call->callee);
      calleeValue = &tempCallee; // NOLINT
    }

    if (!std::holds_alternative<RuntimeValue::Lambda>(calleeValue->data)) {
      throwError(SnowErr::Phase::Evaluator, "attempted to call a non-function value",
                 call->callee->span);
    }

    auto &lambdaVal = std::get<RuntimeValue::Lambda>(calleeValue->data);
    const std::shared_ptr<LambdaInstance> &lambdaInstance = lambdaVal.instance;

    owner->debug.logln("  Calling lambda id=" + std::to_string(lambdaInstance->id));

    std::vector<RuntimeValue> argValues;
    argValues.reserve(call->args.size());
    for (auto &arg : call->args) {
      argValues.push_back(evalExpr(arg));
    }

    RuntimeValue result = runLambda(lambdaInstance, &argValues, thisObject);

    return result;
  }

  // ---------- binary ----------
  case ExprKind::Binary: {
    auto *b = static_cast<RBinaryExpr *>(expr.get());

    RuntimeValueRef lhs = evalExpr(b->left);
    RuntimeValueRef rhs = evalExpr(b->right);

    switch (b->op) {

    case TokenType::LogicalAnd: {
      bool l = SnowlangHelper::RuntimeValueTo<bool>(b->span)(lhs);
      if (!l)
        return {false};
      bool r = SnowlangHelper::RuntimeValueTo<bool>(b->span)(rhs);
      return {r};
    }

    case TokenType::LogicalOr: {
      bool l = SnowlangHelper::RuntimeValueTo<bool>(b->span)(lhs);
      if (l)
        return {true};
      bool r = SnowlangHelper::RuntimeValueTo<bool>(b->span)(rhs);
      return {r};
    }

    case TokenType::Modulo: {
      if (!std::holds_alternative<float>(lhs.data) || !std::holds_alternative<float>(rhs.data)) {
        throwError(SnowErr::Phase::Evaluator, "invalid operands for '%'", b->span,
                   "both operands must be numbers");
      }

      float l = std::get<float>(lhs.data);
      float r = std::get<float>(rhs.data);

      if (r == 0.0f) {
        throwError(SnowErr::Phase::Evaluator, "modulo by zero", b->span);
      }

      return {std::fmod(l, r)};
    }

    case TokenType::Plus: {
      //------ Number sum -----
      if (std::holds_alternative<float>(lhs.data) && std::holds_alternative<float>(rhs.data)) {
        return {std::get<float>(lhs.data) + std::get<float>(rhs.data)};
      }

      //------ String append -----
      if (std::holds_alternative<std::string>(lhs.data) &&
          std::holds_alternative<std::string>(rhs.data)) {
        return {std::get<std::string>(lhs.data) + std::get<std::string>(rhs.data)};
      }

      //------- List append ------
      if (std::holds_alternative<RuntimeValue::List>(lhs.data)) {

        if (std::holds_alternative<RuntimeValue::List>(rhs.data)) {
          RuntimeValue::List lhsList = std::get<RuntimeValue::List>(lhs.data);
          RuntimeValue::List rhsList = std::get<RuntimeValue::List>(rhs.data);
          lhsList.insert(lhsList.end(), rhsList.begin(), rhsList.end());
          return {lhsList};
        }

        RuntimeValue::List lhsList = std::get<RuntimeValue::List>(lhs.data);
        lhsList.push_back(rhs);
        return lhsList;
      } else if (std::holds_alternative<RuntimeValue::List>(rhs.data)) {
        // Expensive O(n)
        RuntimeValue::List rhsList = std::get<RuntimeValue::List>(rhs.data);
        rhsList.insert(rhsList.begin(), lhs);
        return {rhsList};
      }

      throwError(SnowErr::Phase::Evaluator, "invalid operands for '+'", b->span,
                 "expected (number + number) (string + string) (list + any)");
    }

    case TokenType::EqualityComparison: {
      if (SnowlangHelper::isNumericOrBool(lhs) && SnowlangHelper::isNumericOrBool(rhs)) {
        return {SnowlangHelper::toNumber(lhs) == SnowlangHelper::toNumber(rhs)};
      }

      if (std::holds_alternative<std::string>(lhs.data) &&
          std::holds_alternative<std::string>(rhs.data)) {
        return {std::get<std::string>(lhs.data) == std::get<std::string>(rhs.data)};
      }

      return {false};
    }

    default: {
      if (!std::holds_alternative<float>(lhs.data) || !std::holds_alternative<float>(rhs.data)) {
        throwError(SnowErr::Phase::Evaluator, "invalid operands for binary operator", b->span,
                   "both operands must be numbers");
      }

      float l = std::get<float>(lhs.data);
      float r = std::get<float>(rhs.data);

      switch (b->op) {
      case TokenType::Minus:
        return {l - r};
      case TokenType::Star:
        return {l * r};
      case TokenType::Slash:
        if (r == 0.0f) {
          throwError(SnowErr::Phase::Evaluator, "division by zero", b->span);
        }
        return {l / r};

      case TokenType::GreaterThan:
        return {l > r};
      case TokenType::LesserThan:
        return {l < r};
      case TokenType::GreaterOrEqualThan:
        return {l >= r};
      case TokenType::LesserOrEqualThan:
        return {l <= r};

      default:
        throwError(SnowErr::Phase::Evaluator, "unsupported binary operator", b->span);
      }
    }
    }
  }

  // ---------- object ----------
  case ExprKind::Object: {
    auto *o = static_cast<RObjectExpr *>(expr.get());

    ObjectInstance obj;

    for (auto &f : o->fields) {
      obj.fields[f.name] = evalExpr(f.value);
    }

    return {std::make_shared<ObjectInstance>(obj)};
  }

  // ---------- member access ----------
  case ExprKind::MemberAccess: {
    auto *m = static_cast<RMemberAccessExpr *>(expr.get());
    RuntimeValueRef base = evalExpr(m->base);

    if (!std::holds_alternative<ObjectRef>(base.data)) {
      throwError(SnowErr::Phase::Evaluator, "access to non-object", m->span);
    }

    auto &obj = std::get<ObjectRef>(base.data);

    auto it = obj->fields.find(m->member);
    if (it == obj->fields.end()) {
      throwError(SnowErr::Phase::Evaluator, "object has no member '" + m->member + "'", m->span);
    }

    RuntimeValue val = it->second;

    if (std::holds_alternative<RuntimeValue::GameObjectRef>(val.data)) {
      return std::get<RuntimeValue::GameObjectRef>(val.data).getter();
    }

    return val;
  }

  // ---------- this ----------
  case ExprKind::This: {
    CallFrame frame = owner->memory.currentCallFrame();
    if (!frame.thisObject) {
      throwError(SnowErr::Phase::Evaluator, "`this` used outside of a method", expr->span);
    }
    return {frame.thisObject};
  }

  // ---------- Bool ----------
  case ExprKind::Bool: {
    auto *b = static_cast<RBoolExpr *>(expr.get());
    return {b->val};
  }

  // ---------- NULL ----------
  case ExprKind::Null: {
    return {};
  }

  default:
    throwError(SnowErr::Phase::Evaluator, "unsupported expression", expr->span);
  }
}

void Evaluator::execStmt(const RStmtPtr &stmt) {
  switch (stmt->kind) {

  // ---------- expr ----------
  case StmtKind::Expr: {
    auto *s = static_cast<RExprStmt *>(stmt.get());
    evalExpr(s->expr);
    break;
  }

  // ---------- let ----------
  case StmtKind::Let: {
    auto *let = static_cast<RLetStmt *>(stmt.get());

    if (let->assignment) {
      RuntimeValue value = evalExpr(let->assignment);
      tryMemoryWrite(let->location, value, let->span);
    }
    break;
  }

  // ---------- return ----------
  case StmtKind::Return: {
    auto *ret = static_cast<RReturnStmt *>(stmt.get());
    RuntimeValueRef retRes = evalExpr(ret->toReturn);
    owner->debug.logln("[Evaluator] throw ReturnSignal with content: ", retRes);

    throw ReturnSignal{retRes};
  }

  // ---------- continue ----------
  case StmtKind::Continue: {
    throw ContinueSignal{};
  }

  // ---------- break ----------
  case StmtKind::Break: {
    throw BreakSignal{};
  }

  // ---------- import ----------
  case StmtKind::Import: {
    auto *s = static_cast<RImportStmt *>(stmt.get());
    RuntimeValue pathVal = evalExpr(s->path);

    const std::string &path = std::get<std::string>(pathVal.data);
    auto module = owner->loadModule(path);
    break;
  }

  // ---------- while ----------
  case StmtKind::While: {
    auto *whilestmt = static_cast<RWhileStmt *>(stmt.get());

    while (true) {
      RuntimeValueRef conditionRes = evalExpr(whilestmt->condition);
      bool conditionBool = SnowlangHelper::RuntimeValueTo<bool>(whilestmt->span)(conditionRes);
      ;

      if (!conditionBool)
        break;

      try {
        execStmt(whilestmt->doBranch);
      } catch (ContinueSignal &) {
        continue;
      } catch (BreakSignal &) {
        break;
      }
    }
    break;
  }

  // ---------- if ----------
  case StmtKind::If: {
    auto *ifstmt = static_cast<RIfStmt *>(stmt.get());
    RuntimeValueRef conditionRes = evalExpr(ifstmt->condition);

    if (!std::holds_alternative<bool>(conditionRes.data)) {
      throwError(SnowErr::Phase::Evaluator, "if condition must evaluate to a boolean",
                 ifstmt->condition->span);
    }

    auto conditionBool = std::get<bool>(conditionRes.data);

    if (conditionBool) {
      execStmt(ifstmt->thenBranch);
    } else {
      if (ifstmt->elseBranch) {
        execStmt(ifstmt->elseBranch);
      }
    }

    break;
  }

  // ---------- do ----------
  case StmtKind::Do: {
    auto *d = static_cast<RDoStmt *>(stmt.get());

    for (auto &s : d->body) {
      execStmt(s);
    }

    break;
  }

  default:
    throwError(SnowErr::Phase::Evaluator, "unsupported statement", stmt->span);
  }
}

RuntimeValue Evaluator::evalCommand(const RCommandExpr &cmd, const SourceSpan &span) {
  std::vector<RuntimeValue> args;
  args.reserve(cmd.args.size());
  for (auto &arg : cmd.args) {
    args.push_back(evalExpr(arg));
  }

  auto command = tryGetCommandId(cmd.id, span);

  std::unordered_map<std::string, RuntimeValue> flagValues;
  for (auto &[name, expr] : cmd.flags) {
    if (name == "help") {
      owner->io.writeLn(R"(------------------\<color=white\>\<ln\>)" + command.description +
                        R"(\<ln\>\</color\>------------------\<ln\>)");
      return {};
    }

    flagValues[name] = evalExpr(expr);
  }

  return command.function(
      Commands::CommandContext{.snowlang = *owner, .cmd = cmd, .args = args, .flags = flagValues});
}

RuntimeValue Evaluator::runLambda(const std::shared_ptr<LambdaInstance> &lambda,
                                  const std::vector<RuntimeValue> *args, ObjectRef thisObject,
                                  SourceSpan span) {
  owner->memory.pushCallFrame(lambda, std::move(thisObject));

  if (args) {
    const auto &params = lambda->paramSlots;

    if (args->size() != params.size()) {
      throwError(SnowErr::Phase::Evaluator, "Argument Count Mismatch", span);
    }

    for (size_t i = 0; i < params.size(); ++i) {
      owner->memory.write(Location{Location::Type::Local, static_cast<size_t>(params[i])},
                          const_cast<RuntimeValue &>((*args)[i]));
    }
  }

  try {
    for (auto &stmt : lambda->body) {
      execStmt(stmt);
    }
  } catch (const ReturnSignal &r) {
    owner->memory.popCallFrame();
    return r.value;
  }

  owner->memory.popCallFrame();

  return {};
}

} // namespace Snowlang