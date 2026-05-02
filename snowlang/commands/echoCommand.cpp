#include "Commands.hpp"
#include "RuntimeValue.hpp"
#include <string>

namespace Snowlang::Commands {

RuntimeValue echoCommand(const CommandContext &ctx) {
  std::string out;
  for (auto &v : ctx.args) {
    std::visit(
        [&](auto &&val) {
          using T = std::decay_t<decltype(val)>;

          if constexpr (std::is_same_v<T, float>)
            out += std::to_string(val);
          else if constexpr (std::is_same_v<T, int>)
            out += "#" + std::to_string(val);
          else if constexpr (std::is_same_v<T, std::string>)
            out += val;
          else if constexpr (std::is_same_v<T, RuntimeValue::List>)
            out += "<List>";
          else if constexpr (std::is_same_v<T, RuntimeValue::Lambda>)
            out += "<Lambda>";
          else if constexpr (std::is_same_v<T, bool>)
            out += std::string(val ? "true" : "false");
          else if constexpr (std::is_same_v<T, std::monostate>)
            out += "<NULL>";
          else
            throw std::runtime_error("[echo] unsupported type");

          out += " ";
        },
        v.data);
  }

  if (!out.empty())
    out.pop_back();

  return {out};
}

} // namespace Snowlang::Commands