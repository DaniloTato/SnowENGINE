#pragma once

#include "RegistryMacros.hpp"
#include "Scene.hpp"
#include "SceneBuilderRegistry.hpp" // IWYU pragma: keep

class MainScene : public Scene {
public:
  void setup(Scene::Context ctx) override;
};

REGISTER_SCENE_BUILDER("mainScene",
                       []() { return std::make_unique<MainScene>(); });