#include "CommandSignature.hpp"
#include "RuntimeValue.hpp"
#include "SnowlangInstance.hpp"

namespace Snowlang::Commands {

RuntimeValue printCommand(const CommandContext &ctx) {

  bool showType = ctx.getFlag<bool>("type", true);

  if (ctx.hasFlag("jump")) {
    ctx.snowlang.io.writeLn("");
  }

  if (ctx.hasFlag("inline")) {
    for (const RuntimeValue &arg : ctx.args) {
      ctx.snowlang.io.write(arg.toString(showType));
    }
  } else {
    for (const RuntimeValue &arg : ctx.args) {
      ctx.snowlang.io.writeLn(arg.toString(showType));
    }
  }

  return {false};
}

} // namespace Snowlang::Commands
