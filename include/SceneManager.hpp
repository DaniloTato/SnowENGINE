#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include "SceneContext.hpp"

#include "RenderableObject.hpp"

class SceneManager {
public:
  // scene setup fn should be part of a scene class
  using SceneSetupFn = std::function<void(SceneBuilder::SceneContext)>;
  using SceneNameList = std::vector<std::string>;

  void registerScene(const std::string &name, const SceneSetupFn &setup);
  bool loadScene(const std::string &name, Engine &engine);
  void reloadCurrentScene(Engine &engine);
  bool isTransitioning();

  void update(SceneBuilder::SceneContext ctx);

  [[nodiscard]] SceneNameList getRegisteredScenes() const;

  SceneManager() = default;
  SceneManager(const WindowManager &) = delete;
  SceneManager &operator=(const WindowManager &) = delete;
  SceneManager(WindowManager &&) = delete;
  SceneManager &operator=(WindowManager &&) = delete;

private:
  void beginTransition(const std::string &nextScene, Engine &engine);
  void unloadCurrentScene();
  void initFadeOverlay(Engine &engine);

  std::unordered_map<std::string, SceneSetupFn> scenes;
  std::string currentScene;
  std::string queuedScene;

  bool transitioning = false;
  bool fadingOut = true;

  float transitionTimer = 0.f;
  float transitionDuration = 0.5f;

  RenderableObject *fadeOverlay = nullptr;
};