#pragma once

#include <memory>

#include "Cell.hpp"
#include "CommandSignature.hpp"
#include "LambdaInstance.hpp"
#include "Location.hpp"
#include "RuntimeValue.hpp"

namespace Snowlang {

class SnowlangInstance;

struct CallFrame {
  // Watch out. Raw ptr is used on CallFrame for performance reasons.
  LambdaInstance *closure;
  std::vector<CellPtr> locals;
  ObjectRef thisObject;
};

class Memory {
private:
  std::vector<CellPtr> globalSlots;
  std::vector<CallFrame> callStack;
  [[maybe_unused]] SnowlangInstance *owner;

  std::unordered_map<size_t, CommandSignature> commandSlots;

public:
  Memory(SnowlangInstance *owner);

  void write(const Location &location, RuntimeValue &value);
  const RuntimeValue &read(const Location &location);

  [[nodiscard]] const CommandSignature &getCommandById(size_t slot) const;
  void setCommandById(size_t slot, const CommandSignature &value);

  CellPtr readCell(const Location &loc);

  void pushCallFrame(const std::shared_ptr<LambdaInstance> &closure, ObjectRef thisObject);
  void popCallFrame();
  CallFrame &currentCallFrame();
};

} // namespace Snowlang