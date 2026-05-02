#pragma once

#include <cstddef>
#include <cstdint>

namespace Snowlang {

struct Location {
  enum class Type : uint8_t { Local, Global, Capture };

  Type type;
  size_t slot;

  bool isLocal() { return type == Type::Local; }

  Location(Type type, size_t slot) : type(type), slot(slot) {}
  Location() : type(Type::Global), slot(0) {}
};

} // namespace Snowlang