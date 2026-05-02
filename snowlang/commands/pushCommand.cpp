#include "Cell.hpp"
#include "Commands.hpp"
#include "SnowErr.hpp"

namespace Snowlang::Commands {

RuntimeValue pushCommand(const CommandContext &ctx) {
  if (ctx.args.size() != 2) {
    throwError(SnowErr::Phase::Evaluator, "[push] expects 2 args", ctx.cmd.span);
  }

  const RuntimeValue &value = ctx.args[0];
  const RuntimeValue &ref = ctx.args[1];

  CellPtr cell = SnowlangHelper::RuntimeValueTo<CellPtr>(ctx.cmd.span)(ref);

  RuntimeValue &cellValue = cell->getMutableCellValue();

  if (!std::holds_alternative<RuntimeValue::List>(cellValue.data)) {
    throwError(SnowErr::Phase::Evaluator, "[push] expects reference to a list", ctx.cmd.span);
  }

  auto &list = std::get<RuntimeValue::List>(cellValue.data);
  list.push_back(value);

  return {true};
}

} // namespace Snowlang::Commands