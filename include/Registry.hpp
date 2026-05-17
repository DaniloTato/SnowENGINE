#pragma once

#include <iostream>
#include <string>

template <typename Func> class Registry {
public:
  static void registerItem(const std::string &name, Func func) {
    getRegistry()[name] = func;
  }

  static Func get(const std::string &name) {
    auto &registry = getRegistry();

    if (!registry.contains(name)) {
      std::cout << "Registry item not found: " << name << "\n";
      std::cout << "Available items:\n";

      for (auto &[key, _] : registry)
        std::cout << " - " << key << "\n";

      throw std::runtime_error("Registry item not found");
    }

    return registry.at(name);
  }

  static const std::unordered_map<std::string, Func> &getAllItems() {
    return getRegistry();
  }

protected:
  static std::unordered_map<std::string, Func> &getRegistry() {
    static std::unordered_map<std::string, Func> registry;
    return registry;
  }
};