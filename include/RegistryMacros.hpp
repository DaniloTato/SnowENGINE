#pragma once

#define REGISTER_BLUEPRINT(name, func)                                         \
  static bool _blueprint_reg_##__COUNTER__ = []() {                            \
    BlueprintRegistry::registerItem(name, func);                               \
    return true;                                                               \
  }()

#define REGISTER_SCENE_BUILDER(name, func)                                     \
  static bool _scene_builder_reg_##__COUNTER__ = []() {                        \
    SceneBuilderRegistry::registerItem(name, func);                            \
    return true;                                                               \
  }()

#define REGISTER_SCRIPT(name, script, type)                                    \
  static bool _script_register_reg_##__COUNTER__ = []() {                      \
    ScriptRegistry<type>::registerItem(name, NamedScript<type>{name, script}); \
    return true;                                                               \
  }()