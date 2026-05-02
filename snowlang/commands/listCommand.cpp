#include "Commands.hpp"
#include "RuntimeValue.hpp"

namespace Snowlang::Commands {

RuntimeValue listCommand(const CommandContext &ctx) {
  RuntimeValue::List list;
  for (const RuntimeValue &arg : ctx.args) {
    list.push_back(arg);
  }
  return list;
}

} // namespace Snowlang::Commands
