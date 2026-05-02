#include "SharedMemory.hpp"

SharedMemory &SharedMemory::getInstance() {
  static SharedMemory instance;
  return instance;
}

SharedMemory::Key SharedMemory::getKey(std::string_view name) {
  std::string stringName(name);
  if (auto it = names.find(stringName); it != names.end()) {
    return {.id = it->second};
  }

  size_t field = nextField++;
  fields.emplace_back();
  names.emplace(name, field);
  return {.id = field};
}

Snowlang::RuntimeValue SharedMemory::getField(SharedMemory::Key key) {
  return fields[key.id];
}

bool SharedMemory::writeField(std::string_view name,
                              const Snowlang::RuntimeValue &value) {
  std::string stringName(name);
  if (auto it = names.find(stringName); it != names.end()) {
    fields[it->second] = value;
    return true;
  }

  return false;
}