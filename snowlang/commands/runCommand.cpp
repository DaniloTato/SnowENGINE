#include "CommandSignature.hpp"
#include "RuntimeValue.hpp"
#include "SnowlangInstance.hpp"

#include <stdexcept>

namespace Snowlang::Commands {

RuntimeValue runCommand(const CommandContext &ctx) {
  // run will later turn into a statment, not a command
  if (ctx.args.size() != 1)
    throw std::runtime_error("[run] Expected exactly 1 argument");

  if (!std::holds_alternative<std::string>(ctx.args[0].data))
    throw std::runtime_error("[run] Expected a string path");

  const auto &path = std::get<std::string>(ctx.args[0].data);
  std::string source = ctx.snowlang.readFile(path);

  ctx.snowlang.run(source);

  return {};
}

} // namespace Snowlang::Commands