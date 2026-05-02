#include "Commands.hpp"
#include "Helpers.hpp"
#include "SnowlangInstance.hpp"

namespace Snowlang::Commands {

namespace {

std::string getDescription(const std::string &file, const std::string &commandName) {
  std::ifstream in(file);
  if (!in.is_open())
    return "";

  std::ostringstream out;
  std::string line;
  bool inBlock = false;

  const std::string header = "[" + commandName + "]";

  while (std::getline(in, line)) {
    if (!line.empty() && line.front() == '[' && line.back() == ']') {
      if (inBlock) {
        break;
      }
      inBlock = (line == header);
      continue;
    }

    if (inBlock) {
      out << line << '\n';
    }
  }

  return out.str();
}

void newCommand(SnowlangInstance &snowlang, const std::string &name, const CommandFn &signature) {
  size_t id = snowlang.resolver.getNewCommandId(name);
  snowlang.memory.setCommandById(
      id,
      Snowlang::CommandSignature{.function = signature,
                                 .description = getDescription(
                                     Helper::getPath("snowlang/commands/descriptions.txt"), name)});
}

} // namespace

void DefineCommands(Snowlang::SnowlangInstance &snowlang) {
  newCommand(snowlang, "echo", Snowlang::Commands::echoCommand);
  newCommand(snowlang, "list", Snowlang::Commands::listCommand);
  newCommand(snowlang, "push", Snowlang::Commands::pushCommand);
  newCommand(snowlang, "run", Snowlang::Commands::runCommand);
  newCommand(snowlang, "print", Snowlang::Commands::printCommand);
  newCommand(snowlang, "write", Snowlang::Commands::writeCommand);
  newCommand(snowlang, "watch", Snowlang::Commands::watchCommand);
  newCommand(snowlang, "task", Snowlang::Commands::taskCommand);
  newCommand(snowlang, "game_get", Snowlang::Commands::gameGetCommand);
  newCommand(snowlang, "find_class", Snowlang::Commands::findClassCommand);
  newCommand(snowlang, "scene", Snowlang::Commands::sceneCommand);
  newCommand(snowlang, "add_script", Snowlang::Commands::addScriptCommand);
  newCommand(snowlang, "share", Snowlang::Commands::shareCommand);
  newCommand(snowlang, "tile", Snowlang::Commands::tileCommand);
  newCommand(snowlang, "length", Snowlang::Commands::lengthCommand);
  newCommand(snowlang, "read", Snowlang::Commands::readCommand);
}

} // namespace Snowlang::Commands