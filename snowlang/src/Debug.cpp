#include "Debug.hpp"

namespace Snowlang {

Debug::Debug(SnowIO &ioInterface) : io(ioInterface) {}

std::string Debug::locationToString(const Location &location) {
  static const std::array<std::string, 3> typeNames = {"Local", "Global", "Capture"};

  return "Location: " + std::to_string(location.slot) +
         " type: " + typeNames[static_cast<int>(location.type)];
}

} // namespace Snowlang