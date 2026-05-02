#include "Cell.hpp"
#include "Commands.hpp"
#include "RuntimeValue.hpp"
#include "SnowlangHelper.hpp"

namespace Snowlang::Commands {

RuntimeValue watchCommand(const CommandContext &ctx) {

  if (ctx.args.size() < 2) {
    throwError(SnowErr::Phase::Evaluator, "Expected at least 2 args", ctx.cmd.span);
  }

  CellPtr cell = SnowlangHelper::RuntimeValueTo<CellPtr>(ctx.cmd.span)(ctx.args[0]);

  RuntimeValue::Lambda lambda =
      SnowlangHelper::RuntimeValueTo<RuntimeValue::Lambda>(ctx.cmd.span)(ctx.args[1]);

  cell->watchers.push_back(Cell::Watcher{.lambda = lambda.instance, .owner = &ctx.snowlang});

  return {};
}

} // namespace Snowlang::Commands