#include "CommandSignature.hpp"
#include "SnowErr.hpp"
#include "SnowlangInstance.hpp"

namespace Snowlang::Commands {

RuntimeValue readCommand(const CommandContext &ctx) {

  if (ctx.args.size() < 1) {
    throwError(SnowErr::Phase::Evaluator, "Expected file path", ctx.cmd.span);
  }

  const std::string path = ctx.args[0].toString();

  std::string contents;
  bool ok = ctx.snowlang.io.readFile(path, contents);

  if (!ok) {
    throwError(SnowErr::Phase::Evaluator, "read: failed to read file '" + path + "'", ctx.cmd.span);
  }

  return {contents};
}

} // namespace Snowlang::Commands