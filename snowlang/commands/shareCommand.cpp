#include "Commands.hpp"
#include "RuntimeValue.hpp"
#include "SharedMemory.hpp"
#include "SnowlangHelper.hpp"

namespace Snowlang::Commands {

RuntimeValue shareCommand(const CommandContext &ctx) {

  if (ctx.args.size() != 2) {
    throwError(SnowErr::Phase::Evaluator, "[share] expects 2 args", ctx.cmd.span);
  }

  const auto &fieldName = SnowlangHelper::RuntimeValueTo<std::string>(ctx.cmd.span)(ctx.args[0]);
  const RuntimeValue &Value = ctx.args[1];

  if (!SharedMemory::getInstance().writeField(fieldName, Value)) {
    throwError(SnowErr::Phase::Evaluator, "[share] tried to write on a non-existent shared field.",
               ctx.cmd.span);
  }

  return {true};
}

} // namespace Snowlang::Commands