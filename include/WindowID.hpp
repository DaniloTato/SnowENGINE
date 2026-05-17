#pragma once

#include <functional>
#include <limits>

struct WindowID {
  static constexpr std::size_t NullValue =
      std::numeric_limits<std::size_t>::max();
  std::size_t value = NullValue;

  WindowID() = default;
  WindowID(size_t x) { value = x; }

  [[nodiscard]] bool isNull() const { return value == NullValue; }

  explicit operator bool() const { return !isNull(); }

  auto operator<=>(const WindowID &) const = default;
};

namespace std {

template <> struct hash<WindowID> {
  std::size_t operator()(const WindowID &id) const noexcept {
    return std::hash<std::size_t>{}(id.value);
  }
};

} // namespace std