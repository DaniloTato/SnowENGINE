#pragma once
#include "CommandSignature.hpp"
#include "ResolvedAST.hpp"
#include "RuntimeValue.hpp"
#include "Tokenizer.hpp"

namespace Snowlang {

class SnowlangInstance;
struct LValue;

class Evaluator {
private:
  [[maybe_unused]] SnowlangInstance *owner;
  RuntimeValue evalCommand(const RCommandExpr &cmd, const SourceSpan &span);

  struct ReturnSignal {
    RuntimeValue value;
  };

  struct ContinueSignal {
    ContinueSignal() = default;
  };

  struct BreakSignal {
    BreakSignal() = default;
  };

  void tryMemoryWrite(const Location &loc, RuntimeValue &value, const SourceSpan &span);

  RuntimeValue tryMemoryRead(const Location &loc, const SourceSpan &span);

  CellPtr tryMemoryCellRead(const Location &loc, const SourceSpan &span);

  void tryPopCallFrame(const SourceSpan &span);
  CommandSignature tryGetCommandId(size_t slot, const SourceSpan &span);

  LValue evalLValue(const RExprPtr &expr);
  RuntimeValue LValueRead(const RExprPtr &expr, SnowlangInstance &snowlang) const;

public:
  Evaluator(SnowlangInstance *owner);
  RuntimeValue evalExpr(const RExprPtr &);
  RuntimeValue runLambda(const std::shared_ptr<LambdaInstance> &lambda,
                         const std::vector<RuntimeValue> *args = nullptr,
                         ObjectRef thisObject = nullptr, SourceSpan span = SourceSpan{});
  void execStmt(const RStmtPtr &);
};

} // namespace Snowlang