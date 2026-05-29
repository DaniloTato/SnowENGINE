#pragma once

#include <cstddef>
#include <functional>
#include <limits>

struct CameraID {
  static constexpr std::size_t NullValue =
      std::numeric_limits<std::size_t>::max();
  std::size_t value = NullValue;

  CameraID() = default;
  CameraID(size_t x) { value = x; }

  [[nodiscard]] bool isNull() const { return value == NullValue; }

  explicit operator bool() const { return !isNull(); }

  auto operator<=>(const CameraID &) const = default;
};

namespace std {

template <> struct hash<CameraID> {
  std::size_t operator()(const CameraID &id) const noexcept {
    return std::hash<std::size_t>{}(id.value);
  }
};

} // namespace std