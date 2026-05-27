#include "SnowTermIO.hpp"
#include "Terminal.hpp"

namespace Snowlang {

SnowTermIO::SnowTermIO(Terminal &t) : terminal(t) {}

void SnowTermIO::write(const std::string &text) {
  terminal.print("[color=purple]" + text + "[/color]");
}

void SnowTermIO::writeLn(const std::string &text) {
  terminal.printLn("[color=purple]" + text + "[/color]");
}

bool SnowTermIO::hasLine() { return !inputQueue.empty(); }

std::string SnowTermIO::readLine() {
  if (inputQueue.empty())
    return {};

  auto line = std::move(inputQueue.front());
  inputQueue.pop();
  return line;
}

void SnowTermIO::pushLine(std::string line) { inputQueue.push(std::move(line)); }

bool SnowTermIO::writeFile(const std::string &path, const std::string &contents, bool append) {
  std::ofstream file(path, append ? std::ios::app : std::ios::trunc);
  if (!file)
    return false;

  file << contents;
  return true;
}

bool SnowTermIO::readFile(const std::string &path, std::string &contents) {
  std::ifstream file(path, std::ios::in | std::ios::binary);

  if (!file.is_open())
    return false;

  contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

  return true;
}

} // namespace Snowlang