#pragma once
#include "Commands.hpp"
#include "RuntimeValue.hpp"
#include <functional>

namespace Snowlang {

class SnowlangInstance;
struct Executor;

using CommandFn = std::function<RuntimeValue(const Commands::CommandContext &ctx)>;

struct CommandSignature {
  CommandFn function;
  std::string description;
};

} // namespace Snowlang