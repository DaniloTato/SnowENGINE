#pragma once

#include "Tokenizer.hpp"
#include <exception>
#include <string>
#include <vector>

namespace Snowlang {

class SnowErr : public std::exception {
public:
  enum class Phase : std::uint8_t {
    Tokenizer,
    Parser,
    Resolver,
    Evaluator,
    Memory,
    GameObjRefLoading
  };

  struct Diagnostic {
    Phase phase;
    std::string message;
    SourceSpan span;
    std::vector<std::string> notes;
  };

  class MemoryErr : public std::runtime_error {
  public:
    explicit MemoryErr(const std::string &msg) : std::runtime_error(msg) {}
  };

  explicit SnowErr(Diagnostic d);

  [[nodiscard]] const char *what() const noexcept override;

  [[nodiscard]] std::string format(const std::string &source) const;

  [[nodiscard]] const Diagnostic &diagnostic() const { return diag; }

private:
  Diagnostic diag;

  static void printSourceLine(std::ostream &out, const std::string &source, const SourceSpan &span);
};

[[noreturn]] void throwError(SnowErr::Phase phase, const std::string &message,
                             const SourceSpan &span);

[[noreturn]] void throwError(SnowErr::Phase phase, const std::string &message,
                             const SourceSpan &span, const std::vector<std::string> &notes);

[[noreturn]] void throwError(SnowErr::Phase phase, const std::string &message,
                             const SourceSpan &span, const std::string &note);

const char *phaseToString(SnowErr::Phase p);

} // namespace Snowlang