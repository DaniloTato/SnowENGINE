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