#pragma once
#include <unordered_map>
#include <vector>

#include "RuntimeValue.hpp"

class SharedMemory {

private:
  size_t nextField = 0;
  std::unordered_map<std::string, size_t> names;
  std::vector<Snowlang::RuntimeValue> fields;

private:
  explicit SharedMemory() = default;

public:
  struct Key {
    size_t id;
  };

  static SharedMemory &getInstance();
  Key getKey(std::string_view name);
  Snowlang::RuntimeValue getField(Key key);
  bool writeField(std::string_view name, const Snowlang::RuntimeValue &value);
};