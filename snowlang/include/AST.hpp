#pragma once
#include "Node.hpp"
#include "Tokenizer.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Snowlang {

//-------Expressions------
struct Expr : public Node {
  ~Expr() override = default;
};

using ExprPtr = std::shared_ptr<Expr>;

//----Statements-----
struct Stmt : public Node {
  ~Stmt() override = default;
};

using StmtPtr = std::shared_ptr<Stmt>;

//-----Patterns-------
enum class PatternBindMode : std::uint8_t {
  Import,
  LambdaParam,
  Let,
};

struct Pattern : public Node {
  ~Pattern() override = default;
};

using PatternPtr = std::shared_ptr<Pattern>;

//-------------------

struct NumberExpr : Expr {
  float value;
  explicit NumberExpr(float v) : value(v) {}
};

struct IdentifierExpr : Expr {
  std::string name;
  explicit IdentifierExpr(std::string n) : name(std::move(n)) {}
};

struct AddressExpr : Expr {
  std::string name;
  explicit AddressExpr(std::string n) : name(std::move(n)) {}
};

struct CommandExpr : Expr {
  std::string command;
  std::vector<ExprPtr> args;
  std::unordered_map<std::string, ExprPtr> flags;

  CommandExpr(std::string c, std::vector<ExprPtr> a, std::unordered_map<std::string, ExprPtr> f)
      : command(std::move(c)), args(std::move(a)), flags(std::move(f)) {}
};

struct AssignmentExpr : Expr {
  ExprPtr lhs;
  ExprPtr rhs;

  AssignmentExpr(ExprPtr l, ExprPtr r) : lhs(std::move(l)), rhs(std::move(r)) {}
};

struct DerefExpr : Expr {
  ExprPtr inner;
  explicit DerefExpr(ExprPtr i) : inner(std::move(i)) {}
};

struct ParameterExpr : Expr {
  std::string text;
  explicit ParameterExpr(std::string t) : text(std::move(t)) {}
};

struct StringExpr : Expr {
  std::string text;
  explicit StringExpr(std::string t) : text(std::move(t)) {}
};

struct LoadExpr : Expr {
  std::string name;
  explicit LoadExpr(std::string n) : name(std::move(n)) {}
};

struct ThisExpr : Expr {
  ThisExpr() = default;
};

struct LambdaExpr : Expr {
  PatternPtr params;
  std::vector<StmtPtr> body;

  LambdaExpr(PatternPtr params, std::vector<StmtPtr> body)
      : params(std::move(params)), body(std::move(body)) {}
};

struct CallExpr : Expr {
  ExprPtr callee;
  std::vector<ExprPtr> args;

  CallExpr(ExprPtr c, std::vector<ExprPtr> a) : callee(std::move(c)), args(std::move(a)) {}
};

struct BinaryExpr : Expr {
  TokenType op;
  ExprPtr left;
  ExprPtr right;

  BinaryExpr(TokenType o, ExprPtr l, ExprPtr r) : op(o), left(std::move(l)), right(std::move(r)) {}
};

struct BoolExpr : Expr {
  bool val;
  explicit BoolExpr(bool val) : val(val) {}
};

struct NullExpr : Expr {
  explicit NullExpr() = default;
};

struct ExprStmt : Stmt {
  ExprPtr expr;
  explicit ExprStmt(ExprPtr e) : expr(std::move(e)) {}
};

struct LetStmt : Stmt {
  std::string name;
  ExprPtr assignment;
  explicit LetStmt(std::string n, ExprPtr assignment = nullptr)
      : name(std::move(n)), assignment(std::move(assignment)) {}
};

struct ImportStmt : Stmt {
  ExprPtr path;

  ImportStmt(ExprPtr path) : path(std::move(path)) {}
};

struct ReturnStmt : Stmt {
  ExprPtr toReturn;

  ReturnStmt(ExprPtr toReturn) : toReturn(std::move(toReturn)) {}
};

struct BreakStmt : Stmt {
  BreakStmt() = default;
};

struct ContinueStmt : Stmt {
  ContinueStmt() = default;
};

struct IfStmt : Stmt {
  ExprPtr condition;
  StmtPtr thenBranch;
  StmtPtr elseBranch;

  IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch)
      : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
        elseBranch(std::move(elseBranch)) {}
};

struct WhileStmt : Stmt {
  ExprPtr condition;
  StmtPtr doBranch;

  WhileStmt(ExprPtr condition, StmtPtr doBranch)
      : condition(std::move(condition)), doBranch(std::move(doBranch)) {}
};

struct IdentListPattern : Pattern {
  std::vector<std::string> names;

  explicit IdentListPattern(std::vector<std::string> names) : names(std::move(names)) {}
};

struct IndexExpr : Expr {
  ExprPtr base;
  ExprPtr index;

  IndexExpr(ExprPtr base, ExprPtr index) : base(std::move(base)), index(std::move(index)) {}
};

struct DoStmt : Stmt {
  std::vector<StmtPtr> body;

  explicit DoStmt(std::vector<StmtPtr> body) : body(std::move(body)) {}
};

struct ObjectExpr : Expr {
  struct Field {
    std::string name;
    ExprPtr value;

    Field(std::string n, ExprPtr v) : name(std::move(n)), value(std::move(v)) {}
  };

  std::vector<Field> fields;

  explicit ObjectExpr(std::vector<Field> f) : fields(std::move(f)) {}
};

struct MemberAccessExpr : Expr {
  ExprPtr base;
  std::string member;

  MemberAccessExpr(ExprPtr b, std::string m) : base(std::move(b)), member(std::move(m)) {}
};

} // namespace Snowlang