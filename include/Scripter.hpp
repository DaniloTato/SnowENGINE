#pragma once
#include <any>
#include <string>
#include <unordered_map>

#include "GameObjectExposure.hpp"
#include "ScriptRegistry.hpp"

struct GeneralContext;

template <typename OwnerType> class Scripter {
public:
  using ScriptFunc = void (*)(OwnerType &, const GeneralContext &);

  struct NamedScript {
    std::string name;
    ScriptFunc function;
    bool enabled = true;
  };

  /*Script States*/
  std::unordered_map<std::string, std::any> scriptState;

  template <typename T, typename... Args>
  T &getState(const std::string &key, Args &&...args) {
    auto &stateAny = scriptState[key];
    if (!stateAny.has_value()) {
      stateAny = T(std::forward<Args>(args)...);
    }
    return std::any_cast<T &>(stateAny);
  }
  /*Script States*/

  void addScript(Scripter::ScriptFunc function) { 
    scripts.push_back(ScriptRegistry<OwnerType>::get()); }

  void runScripts(OwnerType &owner, const GeneralContext &ctx) {
    for (auto &script : scripts) {
      if (script.function && script.enabled) {
        script.function(owner, ctx);
      }
    }
  }

  GameObjectExposure::Value::Object describe() {
    auto desc = std::make_shared<GameObjectExposure::Descriptor>();

    for (auto &script : scripts) {
      bool &enabledVal = script.enabled;
      desc->fields[script.name] =
          GameObjectExposure::makePublicField<bool>(enabledVal);
    }

    return desc;
  }

private:
  std::vector<NamedScript> scripts;

  static inline void toLowercase(std::string &s) {
    std::ranges::transform(s, s.begin(),
                           [](unsigned char c) { return std::tolower(c); });
  }
};