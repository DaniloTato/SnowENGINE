#pragma once

#include <algorithm>
#include <stdexcept>

#include "NamedScript.hpp"
#include "Registry.hpp"

template <typename OwnerType>
class ScriptRegistry : public Registry<NamedScript<OwnerType>> {
  using Base = Registry<NamedScript<OwnerType>>;

public:
  static std::string findSecond(ScriptFunc<OwnerType> target) {
    auto &registry = Base::getRegistry();
    auto it =
        std::find_if(registry.begin(), registry.end(), [&](const auto &pair) {
          return pair.second.function == target;
        });
    if (it == registry.end()) {
      throw std::runtime_error("Function not found");
    }
    return it->first;
  }
};