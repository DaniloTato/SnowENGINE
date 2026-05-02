#include "ParserPrint.hpp"

#include <cstddef>
#include <string>
#include <typeinfo>

namespace Snowlang::ParserPrint {

static void indentPad(SnowIO &io, size_t indent) {
  for (size_t i = 0; i < indent; ++i)
    io.write("  ");
}

void printPattern(SnowIO &io, const PatternPtr &pattern, size_t indent) {
  std::string pad(indent * 2, ' ');

  if (!pattern) {
    io.writeLn(pad + "<null pattern>");
    return;
  }

  if (auto list = dynamic_cast<IdentListPattern *>(pattern.get())) {
    io.writeLn(pad + "IdentListPattern");
    for (const auto &name : list->names) {
      std::string line = pad;
      line += "  ";
      line += name;
      io.writeLn(line);
    }
    return;
  }

  io.writeLn(pad + "<unknown pattern>");
}

void printStmt(SnowIO &io, const StmtPtr &stmt, size_t indent) {
  if (!stmt) {
    indentPad(io, indent);
    io.writeLn("<null stmt>");
    return;
  }

  // -------- ExprStmt --------
  if (auto *s = dynamic_cast<ExprStmt *>(stmt.get())) {
    indentPad(io, indent);
    io.writeLn("ExprStmt");
    printExpr(io, s->expr, indent + 1);
    return;
  }

  // -------- LetStmt --------
  if (auto *s = dynamic_cast<LetStmt *>(stmt.get())) {
    indentPad(io, indent);
    io.writeLn("LetStmt " + s->name + ":");
    printExpr(io, s->assignment, indent + 1);
    return;
  }

  // -------- ReturnStmt --------
  if (auto *s = dynamic_cast<ReturnStmt *>(stmt.get())) {
    indentPad(io, indent);
    io.writeLn("ReturnStmt");
    printExpr(io, s->toReturn, indent + 1);
    return;
  }

  indentPad(io, indent);
  io.writeLn(std::string("<Unknown Stmt: ") + typeid(stmt.get()).name() + ">");
}

void printExpr(SnowIO &io, const ExprPtr &expr, size_t indent) {
  if (!expr) {
    indentPad(io, indent);
    io.writeLn("<null expr>");
    return;
  }

  // -------- Literals --------
  if (auto n = dynamic_cast<NumberExpr *>(expr.get())) {
    indentPad(io, indent);
    io.writeLn("Number(" + std::to_string(n->value) + ")");
    return;
  }

  if (auto s = dynamic_cast<StringExpr *>(expr.get())) {
    indentPad(io, indent);
    io.writeLn("String(\"" + s->text + "\")");
    return;
  }

  // -------- Variables --------
  if (auto p = dynamic_cast<ParameterExpr *>(expr.get())) {
    indentPad(io, indent);
    io.writeLn("Parameter(\"" + p->text + "\")");
    return;
  }

  if (auto l = dynamic_cast<LoadExpr *>(expr.get())) {
    indentPad(io, indent);
    io.writeLn("Load(\"" + l->name + "\")");
    return;
  }

  // -------- Unary --------
  if (auto d = dynamic_cast<DerefExpr *>(expr.get())) {
    indentPad(io, indent);
    io.writeLn("Deref");
    printExpr(io, d->inner, indent + 1);
    return;
  }

  // -------- Assignment --------
  if (auto a = dynamic_cast<AssignmentExpr *>(expr.get())) {
    indentPad(io, indent);
    io.writeLn("Assignment");

    indentPad(io, indent + 1);
    io.writeLn("LHS:");
    printExpr(io, a->lhs, indent + 2);

    indentPad(io, indent + 1);
    io.writeLn("RHS:");
    printExpr(io, a->rhs, indent + 2);

    return;
  }

  // -------- Binary --------
  if (auto b = dynamic_cast<BinaryExpr *>(expr.get())) {
    indentPad(io, indent);
    io.write("Binary(");

    switch (b->op) {
    case TokenType::Plus:
      io.write("+");
      break;
    case TokenType::Minus:
      io.write("-");
      break;
    case TokenType::Star:
      io.write("*");
      break;
    case TokenType::Slash:
      io.write("/");
      break;
    case TokenType::EqualityComparison:
      io.write("==");
      break;
    case TokenType::GreaterThan:
      io.write(">");
      break;
    case TokenType::LesserThan:
      io.write("<");
      break;
    case TokenType::GreaterOrEqualThan:
      io.write(">=");
      break;
    case TokenType::LesserOrEqualThan:
      io.write("<=");
      break;
    default:
      io.write("?");
      break;
    }

    io.writeLn(")");
    printExpr(io, b->left, indent + 1);
    printExpr(io, b->right, indent + 1);
    return;
  }

  // -------- Object --------
  if (auto o = dynamic_cast<ObjectExpr *>(expr.get())) {
    indentPad(io, indent);
    io.writeLn("Object");

    for (const auto &field : o->fields) {
      indentPad(io, indent + 1);
      io.writeLn("Field \"" + field.name + "\":");
      printExpr(io, field.value, indent + 2);
    }

    return;
  }

  // -------- Member Access --------
  if (auto m = dynamic_cast<MemberAccessExpr *>(expr.get())) {
    indentPad(io, indent);
    io.writeLn("MemberAccess");

    indentPad(io, indent + 1);
    io.writeLn("Base:");
    printExpr(io, m->base, indent + 2);

    indentPad(io, indent + 1);
    io.writeLn("Member: \"" + m->member + "\"");

    return;
  }

  // -------- Command --------
  if (auto c = dynamic_cast<CommandExpr *>(expr.get())) {
    indentPad(io, indent);
    io.writeLn("Command(\"" + c->command + "\")");

    for (size_t i = 0; i < c->args.size(); ++i) {
      indentPad(io, indent + 1);
      io.writeLn("Arg[" + std::to_string(i) + "]:");
      printExpr(io, c->args[i], indent + 2);
    }

    if (!c->flags.empty()) {
      indentPad(io, indent + 1);
      io.writeLn("Flags:");
      for (const auto &[key, value] : c->flags) {
        indentPad(io, indent + 2);
        io.writeLn(key + ":");
        printExpr(io, value, indent + 3);
      }
    }

    return;
  }

  // -------- Call --------
  if (auto call = dynamic_cast<CallExpr *>(expr.get())) {
    indentPad(io, indent);
    io.writeLn("Call");

    indentPad(io, indent + 1);
    io.writeLn("Callee:");
    printExpr(io, call->callee, indent + 2);

    for (size_t i = 0; i < call->args.size(); ++i) {
      indentPad(io, indent + 1);
      io.writeLn("Arg[" + std::to_string(i) + "]:");
      printExpr(io, call->args[i], indent + 2);
    }

    return;
  }

  // -------- Lambda --------
  if (auto l = dynamic_cast<LambdaExpr *>(expr.get())) {
    indentPad(io, indent);
    io.writeLn("Lambda");

    indentPad(io, indent + 1);
    io.writeLn("Params:");
    printPattern(io, l->params, indent + 2);

    indentPad(io, indent + 1);
    io.writeLn("Body:");
    for (size_t i = 0; i < l->body.size(); ++i) {
      indentPad(io, indent + 2);
      io.writeLn("Stmt[" + std::to_string(i) + "]:");
      printStmt(io, l->body[i], indent + 3);
    }

    return;
  }

  indentPad(io, indent);
  io.writeLn("<Unknown Expr>");
}

} // namespace Snowlang::ParserPrint