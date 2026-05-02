#include "CommandSignature.hpp"
#include "SnowErr.hpp"
#include "SnowlangInstance.hpp"

namespace Snowlang::Commands {

RuntimeValue writeCommand(const CommandContext &ctx) {

  if (ctx.args.size() < 2) {
    throwError(SnowErr::Phase::Evaluator, "Expected at least 2 args", ctx.cmd.span);
  }

  const std::string path = ctx.args[0].toString();
  const std::string text = ctx.args[1].toString();

  bool append = ctx.getFlag<bool>("append", true);

  if (ctx.hasFlag("noappend")) {
    append = false;
  }

  std::string lnJump = "";
  if (ctx.hasFlag("jump")) {
    lnJump = "\n";
  }

  bool ok = ctx.snowlang.io.writeFile(path, text + lnJump, append);

  if (!ok) {
    ctx.snowlang.debug.logln("write: failed to write file '", path, "'");
  }

  return {ok};
}

} // namespace Snowlang::Commands