#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include "RenderableObject.hpp"

class SceneManager {
public:
  using SceneSetupFn = std::function<void()>;
  using SceneNameList = std::vector<std::string>;

  static SceneManager &getInstance();

  void registerScene(const std::string &name, const SceneSetupFn &setup);
  bool loadScene(const std::string &name);
  void reloadCurrentScene();
  bool isTransitioning();

  void update();

  [[nodiscard]] SceneNameList getRegisteredScenes() const;

private:
  SceneManager() = default;

  void beginTransition(const std::string &nextScene);
  void unloadCurrentScene();
  void initFadeOverlay();

  std::unordered_map<std::string, SceneSetupFn> scenes;
  std::string currentScene;
  std::string queuedScene;

  bool transitioning = false;
  bool fadingOut = true;

  float transitionTimer = 0.f;
  float transitionDuration = 0.5f;

  RenderableObject *fadeOverlay = nullptr;
};