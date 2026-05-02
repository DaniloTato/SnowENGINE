#pragma once
#include "RuntimeValue.hpp"
#include "SnowlangHelper.hpp"

namespace Snowlang::Commands {
using CommandFlags = std::unordered_map<std::string, RuntimeValue>;

struct CommandContext {
  SnowlangInstance &snowlang;
  const RCommandExpr &cmd;
  const std::vector<RuntimeValue> &args;
  const CommandFlags &flags;

  [[nodiscard]] bool hasFlag(const std::string &name) const {
    return flags.find(name) != flags.end();
  }

  template <typename T> T getFlag(const std::string &name, const T &defaultValue) const {
    auto it = flags.find(name);
    if (it == flags.end() || it->second.isNull()) {
      return defaultValue;
    }

    return SnowlangHelper::RuntimeValueTo<T>(cmd.span)(it->second);
  }
};

RuntimeValue echoCommand(const CommandContext &ctx);
RuntimeValue listCommand(const CommandContext &ctx);
RuntimeValue runCommand(const CommandContext &ctx);
RuntimeValue printCommand(const CommandContext &ctx);
RuntimeValue writeCommand(const CommandContext &ctx);
RuntimeValue watchCommand(const CommandContext &ctx);
RuntimeValue taskCommand(const CommandContext &ctx);
RuntimeValue gameGetCommand(const CommandContext &ctx);
RuntimeValue findClassCommand(const CommandContext &ctx);
RuntimeValue pushCommand(const CommandContext &ctx);
RuntimeValue sceneCommand(const CommandContext &ctx);
RuntimeValue addScriptCommand(const CommandContext &ctx);
RuntimeValue shareCommand(const CommandContext &ctx);
RuntimeValue tileCommand(const CommandContext &ctx);
RuntimeValue lengthCommand(const CommandContext &ctx);
RuntimeValue readCommand(const CommandContext &ctx);
void DefineCommands(SnowlangInstance &snowlang);
} // namespace Snowlang::Commands