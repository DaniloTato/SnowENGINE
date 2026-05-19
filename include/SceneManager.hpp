#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include "Engine.hpp"

#include "RenderableObject.hpp"

class SceneManager {
public:
  // scene setup fn should be part of a scene class
  using SceneSetupFn = std::function<void(Engine &)>;
  using SceneNameList = std::vector<std::string>;

  static SceneManager &getInstance();

  void registerScene(const std::string &name, const SceneSetupFn &setup);
  bool loadScene(const std::string &name, WindowManager &windowManager);
  void reloadCurrentScene(WindowManager &windowManager);
  bool isTransitioning();

  void update(Engine &engine);

  [[nodiscard]] SceneNameList getRegisteredScenes() const;

private:
  SceneManager() = default;

  void beginTransition(const std::string &nextScene,
                       WindowManager &windowManager);
  void unloadCurrentScene();
  void initFadeOverlay(WindowManager &windowManager);

  std::unordered_map<std::string, SceneSetupFn> scenes;
  std::string currentScene;
  std::string queuedScene;

  bool transitioning = false;
  bool fadingOut = true;

  float transitionTimer = 0.f;
  float transitionDuration = 0.5f;

  RenderableObject *fadeOverlay = nullptr;
};