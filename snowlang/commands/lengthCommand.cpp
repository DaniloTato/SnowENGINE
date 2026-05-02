#include "Commands.hpp"
#include "RuntimeValue.hpp"

namespace Snowlang::Commands {

RuntimeValue lengthCommand(const CommandContext &ctx) {

  if (ctx.args.size() != 1) {
    throwError(SnowErr::Phase::Evaluator, "[length] expects 1 arg", ctx.cmd.span);
  }

  if (std::holds_alternative<RuntimeValue::List>(ctx.args[0].data)) {
    auto list = std::get<RuntimeValue::List>(ctx.args[0].data);
    return {static_cast<float>(list.size())};
  } else if (std::holds_alternative<std::string>(ctx.args[0].data)) {
    auto str = std::get<std::string>(ctx.args[0].data);
    return {static_cast<float>(str.length())};
  }

  throwError(SnowErr::Phase::Evaluator, "[length] invalid data type", ctx.cmd.span);

  return {};
}

} // namespace Snowlang::Commands
