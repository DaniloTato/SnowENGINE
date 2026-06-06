#pragma once

#include "RegistryMacros.hpp"
#include "Scene.hpp"
#include "SceneBuilderRegistry.hpp" // IWYU pragma: keep
#include "Terminal.hpp"

class TerminalScene : public Scene, public IEventListener {
public:
  void setup(Scene::Context ctx) override;
  void update(const GeneralContext &ctx) override;
  void handleEvent(WindowID, const sf::Event &) override;

private:
  Terminal terminal;
  CameraComponent *camera = nullptr;
  GameObject *text = nullptr;
  WindowID terminalWindow;
  std::string lastMarkup;
};

REGISTER_SCENE_BUILDER("terminalScene",
                       []() { return std::make_unique<TerminalScene>(); });