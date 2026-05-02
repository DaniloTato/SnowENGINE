#include "Memory.hpp"
#include "Debug.hpp"
#include "LambdaInstance.hpp"
#include "SnowErr.hpp"
#include "SnowlangInstance.hpp"

namespace Snowlang {

Memory::Memory(SnowlangInstance *owner) : owner(owner) {}

const RuntimeValue &Memory::read(const Location &loc) {
  owner->debug.logln("[Memory] Reading ===>", Debug::locationToString(loc));

  switch (loc.type) {
  case Location::Type::Global: {
    if (loc.slot >= globalSlots.size() || !globalSlots[loc.slot]) {
      throw SnowErr::MemoryErr("uninitialized global variable");
    }
    return globalSlots[loc.slot]->getCellValue();
  }

  case Location::Type::Local: {
    auto &frame = currentCallFrame();
    if (loc.slot >= frame.locals.size())
      throw SnowErr::MemoryErr("local slot out of range");
    return frame.locals[loc.slot]->getCellValue();
  }

  case Location::Type::Capture: {
    auto &frame = currentCallFrame();
    if (loc.slot >= frame.closure->captures.size())
      throw SnowErr::MemoryErr("capture slot out of range");
    return frame.closure->captures[loc.slot]->getCellValue();
  }
  }

  throw SnowErr::MemoryErr("invalid memory location type");
}

void Memory::write(const Location &location, RuntimeValue &value) {
  switch (location.type) {
  case Location::Type::Global: {
    if (location.slot >= globalSlots.size()) {
      globalSlots.resize(location.slot + 1);
    }

    auto &slot = globalSlots[location.slot];
    if (!slot) {
      slot = std::make_shared<Cell>();
    }

    slot->setCellValue(value);
    break;
  }

  case Location::Type::Local: {
    auto &frame = currentCallFrame();
    if (location.slot >= frame.locals.size()) {
      throw SnowErr::MemoryErr("local slot out of range");
    }
    frame.locals[location.slot]->setCellValue(value);
    break;
  }

  case Location::Type::Capture: {
    auto &frame = currentCallFrame();
    if (location.slot >= frame.closure->captures.size()) {
      throw SnowErr::MemoryErr("capture slot out of range");
    }
    frame.closure->captures[location.slot]->setCellValue(value);
    break;
  }
  }
}

void Memory::pushCallFrame(const std::shared_ptr<LambdaInstance> &closure, ObjectRef thisObject) {
  CallFrame frame;
  frame.closure = closure.get();
  frame.locals.resize(closure->frameSize);

  for (auto &cell : frame.locals) {
    cell = std::make_shared<Cell>();
  }

  frame.thisObject = std::move(thisObject);
  callStack.push_back(std::move(frame));
}

void Memory::popCallFrame() {
  if (callStack.empty()) {
    throw SnowErr::MemoryErr("call stack underflow");
  }
  callStack.pop_back();
}

CallFrame &Memory::currentCallFrame() {
  if (callStack.empty()) {
    throw SnowErr::MemoryErr("no active call frame");
  }
  return callStack.back();
}

const CommandSignature &Memory::getCommandById(size_t slot) const {
  auto it = commandSlots.find(slot);
  if (it == commandSlots.end()) {
    throw SnowErr::MemoryErr("invalid command id " + std::to_string(slot));
  }
  return it->second;
}

void Memory::setCommandById(size_t slot, const CommandSignature &value) {
  commandSlots.emplace(slot, value);
}

CellPtr Memory::readCell(const Location &loc) {
  switch (loc.type) {
  case Location::Type::Global:
    return globalSlots[loc.slot];

  case Location::Type::Local: {
    auto &frame = currentCallFrame();
    return frame.locals.at(loc.slot);
  }

  case Location::Type::Capture: {
    auto &frame = currentCallFrame();
    return frame.closure->captures.at(loc.slot);
  }
  }

  throw SnowErr::MemoryErr("invalid location");
}

} // namespace Snowlang