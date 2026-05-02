#pragma once
#include "Node.hpp"
#include "Resolver.hpp"
#include <memory>
#include <string>

namespace Snowlang {

struct CaptureDesc {
  CaptureDesc() = default;
  Location source;
  CaptureDesc(Location source) : source(source) {};
};

enum class StmtKind : std::uint8_t { Expr, Import, Let, Return, If, Do, While, Break, Continue };

enum class ExprKind : std::uint8_t {
  Number,
  String,
  VarRef,
  Address,
  Assign,
  Binary,
  Call,
  Lambda,
  Block,
  Command,
  Addr,
  Deref,
  Load,
  Indexing,
  Object,
  MemberAccess,
  This,
  Bool,
  Null
};

struct RExpr : public Node {
  ExprKind kind;
  explicit RExpr(ExprKind kind) : kind(kind) {}
  ~RExpr() override = default;
};

using RExprPtr = std::shared_ptr<RExpr>;

//------------ statements -------
struct RStmt : public Node {
  StmtKind kind;
  explicit RStmt(StmtKind kind) : kind(kind) {}
  ~RStmt() override = default;
};

using RStmtPtr = std::shared_ptr<RStmt>;

struct RExprStmt : RStmt {
  RExprPtr expr;

  explicit RExprStmt(RExprPtr e) : RStmt(StmtKind::Expr), expr(std::move(e)) {}
};

struct RReturnStmt : RStmt {
  RExprPtr toReturn;

  RReturnStmt(RExprPtr toReturn) : RStmt(StmtKind::Return), toReturn(std::move(toReturn)) {}
};

struct RBreakStmt : RStmt {
  RBreakStmt() : RStmt(StmtKind::Break) {}
};

struct RContinueStmt : RStmt {
  RContinueStmt() : RStmt(StmtKind::Continue) {}
};

// ---------- literals ----------
struct RNumberExpr : RExpr {
  float value;
  explicit RNumberExpr(float v) : RExpr(ExprKind::Number), value(v) {}
};

struct RStringExpr : RExpr {
  std::string value;
  explicit RStringExpr(std::string v) : RExpr(ExprKind::String), value(std::move(v)) {}
};

// ---------- assignment ----------
struct RAssignExpr : RExpr {
  RExprPtr lhs;
  RExprPtr rhs;

  RAssignExpr(RExprPtr l, RExprPtr r)
      : RExpr(ExprKind::Assign), lhs(std::move(l)), rhs(std::move(r)) {}
};

struct RVarRefExpr : RExpr {
  Location location;
  explicit RVarRefExpr(Location location) : RExpr(ExprKind::VarRef), location(location) {}
};

struct RAddressExpr : RExpr {
  Location location;
  explicit RAddressExpr(Location location) : RExpr(ExprKind::Address), location(location) {}
};

struct RLetStmt : RStmt {
  Location location;
  RExprPtr assignment;
  explicit RLetStmt(Location location, RExprPtr assignment)
      : RStmt(StmtKind::Let), location(location), assignment(std::move(assignment)) {}
};

// ---------- commands ----------
struct RCommandExpr : RExpr {
  int id;
  std::vector<RExprPtr> args;
  std::unordered_map<std::string, RExprPtr> flags;

  RCommandExpr(int id, std::vector<RExprPtr> a, std::unordered_map<std::string, RExprPtr> f)
      : RExpr(ExprKind::Command), id(id), args(std::move(a)), flags(std::move(f)) {}
};

// ---------- lambdas ----------
struct RLambdaExpr : RExpr {
  int id;
  std::vector<size_t> paramSlots;
  std::vector<RStmtPtr> body;
  int frameSize;

  std::vector<CaptureDesc> captures;

  RLambdaExpr(int id, std::vector<size_t> params, std::vector<RStmtPtr> body, int frameSize,
              std::vector<CaptureDesc> captures)
      : RExpr(ExprKind::Lambda), id(id), paramSlots(std::move(params)), body(std::move(body)),
        frameSize(frameSize), captures(std::move(captures)) {}
};

// ---------- calls ----------
struct RCallExpr : RExpr {
  RExprPtr callee;
  std::vector<RExprPtr> args;

  RCallExpr(RExprPtr c, std::vector<RExprPtr> a)
      : RExpr(ExprKind::Call), callee(std::move(c)), args(std::move(a)) {}
};

// ---------- binary ----------
struct RBinaryExpr : RExpr {
  TokenType op;
  RExprPtr left;
  RExprPtr right;

  RBinaryExpr(TokenType o, RExprPtr l, RExprPtr r)
      : RExpr(ExprKind::Binary), op(o), left(std::move(l)), right(std::move(r)) {}
};

//--------- if ---------
struct RIfStmt : RStmt {
  RExprPtr condition;
  RStmtPtr thenBranch;
  RStmtPtr elseBranch;

  RIfStmt(RExprPtr condition, RStmtPtr thenBranch, RStmtPtr elseBranch)
      : RStmt(StmtKind::If), condition(std::move(condition)), thenBranch(std::move(thenBranch)),
        elseBranch(std::move(elseBranch)) {}
};

//--------- while ---------
struct RWhileStmt : RStmt {
  RExprPtr condition;
  RStmtPtr doBranch;

  RWhileStmt(RExprPtr condition, RStmtPtr doBranch)
      : RStmt(StmtKind::While), condition(std::move(condition)), doBranch(std::move(doBranch)) {}
};

//--------- do ---------
struct RDoStmt : RStmt {
  std::vector<RStmtPtr> body;

  explicit RDoStmt(std::vector<RStmtPtr> body) : RStmt(StmtKind::Do), body(std::move(body)) {}
};

// ----------- import -----------
struct RImportStmt : RStmt {
  RExprPtr path;

  RImportStmt(RExprPtr p) : RStmt(StmtKind::Import), path(std::move(p)) {}
};

// ---------- indexing [] --------
struct RIndexExpr : RExpr {
  RExprPtr base;
  RExprPtr index;

  RIndexExpr(RExprPtr base, RExprPtr index)
      : RExpr(ExprKind::Indexing), base(std::move(base)), index(std::move(index)) {}
};

// ---------- Objects --------
struct RObjectExpr : RExpr {
  struct Field {
    std::string name;
    RExprPtr value;
  };

  std::vector<Field> fields;

  RObjectExpr(std::vector<Field> f, SourceSpan s) : RExpr(ExprKind::Object), fields(std::move(f)) {
    kind = ExprKind::Object;
    span = s;
  }
};

// ---------- Object members --------
struct RMemberAccessExpr : RExpr {
  RExprPtr base;
  std::string member;

  std::optional<Location> baseLocation;

  RMemberAccessExpr(RExprPtr base, std::string member, std::optional<Location> baseLocation,
                    SourceSpan span)
      : RExpr(ExprKind::MemberAccess), base(std::move(base)), member(std::move(member)),
        baseLocation(baseLocation) {
    kind = ExprKind::MemberAccess;
    this->span = span;
  }
};

struct RThisExpr : RExpr {
  RThisExpr() : RExpr(ExprKind::This) {}
};

struct RBoolExpr : RExpr {
  bool val;
  explicit RBoolExpr(bool val) : RExpr(ExprKind::Bool), val(val) {}
};

struct RNullExpr : RExpr {
  explicit RNullExpr() : RExpr(ExprKind::Null) {}
};

} // namespace Snowlang