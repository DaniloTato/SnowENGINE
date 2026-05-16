#pragma once
#include "Location.hpp"
#include "SnowIO.hpp"
#include <sstream>
#include <string>

#include <type_traits>
#include <utility>

// quite the interesting little piece of code
// basically a compile time boolean

/*typename = void is only there to reject malformed specializations
of the stuct*/
template <typename, typename = void> struct has_toString : std::false_type {};

/*we make a compiler-time-only object of Type const T, then check if it has .toString() method.
If the process is successful, decltype returns the type of our compiler-time-only object. And, thus,
we turn it into a void type to satisfy the template's needs*/
template <typename T>
struct has_toString<T, std::void_t<decltype(std::declval<const T &>().toString())>>
    : std::true_type {};

namespace Snowlang {

struct Debug {
#ifdef DEBUG
  static constexpr bool enabled = true;
#else
  static constexpr bool enabled = false;
#endif

  Debug(SnowIO &ioInterface);
  SnowIO &io;

  template <typename... Args> void log(Args &&...args) {
    if (!enabled)
      return;
    io.write(concat(std::forward<Args>(args)...));
  }

  template <typename... Args> void logln(Args &&...args) {
    if (!enabled)
      return;
    io.writeLn(concat(std::forward<Args>(args)...));
  }

  static std::string locationToString(const Location &location);

private:
  template <typename T> void append(std::ostringstream &oss, T &&value) {
    // Compile time conditional
    if constexpr (has_toString<std::decay_t<T>>::value) {
      oss << value.toString();
    } else {
      oss << std::forward<T>(value);
    }
  }

  template <typename... Args> std::string concat(Args &&...args) {
    std::ostringstream oss;
    (append(oss, std::forward<Args>(args)), ...);
    return oss.str();
  }
};

} // namespace Snowlang