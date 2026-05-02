#pragma once

#include "AST.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Location.hpp"

namespace Snowlang {

class SnowlangInstance;

using PatternPtr = std::shared_ptr<Pattern>;
using ExprPtr = std::shared_ptr<Expr>;
using StmtPtr = std::shared_ptr<Stmt>;

struct RExpr;
struct RStmt;

using RExprPtr = std::shared_ptr<RExpr>;
using RStmtPtr = std::shared_ptr<RStmt>;

class Resolver {
private:
  [[maybe_unused]] SnowlangInstance *owner;
  // ---- Symbol tables ----
  std::unordered_map<std::string, size_t> globalSymbols;
  size_t nextGlobalSlot = 0;
  std::vector<std::unordered_map<std::string, size_t>> localSymbolsStack;
  std::vector<size_t> localSlotCounters;

  struct CaptureInfo {
    size_t captureSlot;
    Location sourceLocation;

    CaptureInfo(size_t captureSlot, Location sourceLocation)
        : captureSlot(captureSlot), sourceLocation(sourceLocation) {}
  };

  struct LambdaScope {
    std::unordered_map<std::string, CaptureInfo> captures;
    size_t nextCaptureSlot = 0;
  };

  std::vector<LambdaScope> lambdaStack;

public:
  explicit Resolver(SnowlangInstance *owner);

  //----- Commands ------
  size_t getNewCommandId(const std::string &name);
  std::unordered_map<std::string, size_t> commandIds;
  size_t nextCommandId = 0;

  // ---- Lambdas ----
  size_t nextLambdaId = 0;

  RStmtPtr resolveStmt(const StmtPtr &stmt);
  RExprPtr resolveExpr(const ExprPtr &expr);

  Location resolveCall(const std::string &name, const SourceSpan &span);
  Location resolveVarCreation(const std::string &name, const SourceSpan &span);

  void dumpLambdaStack(const std::vector<LambdaScope> &stack, const std::string &var);

  void leaveLocalScope();
  void enterLocalScope();

  [[nodiscard]] const std::unordered_map<std::string, size_t> &getGlobalSymbols() const;

  void resolvePattern(const PatternPtr &pattern, PatternBindMode mode,
                      std::vector<size_t> &outSlots);

  bool isAssignable(const RExprPtr &expr);
};

} // namespace Snowlang