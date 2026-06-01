#pragma once

#include "RegistryMacros.hpp"
#include "Scene.hpp"
#include "SceneBuilderRegistry.hpp"

class MainScene : public Scene {
public:
  void setup(Scene::Context ctx) override;
};

REGISTER_SCENE_BUILDER("mainScene",
                       []() { return std::make_unique<MainScene>(); });