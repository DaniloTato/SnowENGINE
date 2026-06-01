#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

struct GeneralContext;

#include "Scene.hpp"

class Engine;

class SceneManager {
public:
  using SceneFactory = std::function<std::unique_ptr<Scene>()>;
  using SceneNameList = std::vector<std::string>;
  void registerScene(const std::string &name, SceneFactory factory);
  bool loadScene(const std::string &name, Engine &engine);
  void reloadCurrentScene(Engine &engine);
  void update(const GeneralContext &ctx, const Scene::Context &sceneContext);
  Scene *getCurrentScene();
  bool isTransitioning();

  [[nodiscard]] SceneNameList getRegisteredScenes() const;

private:
  void beginTransition(const std::string &nextScene, Engine &engine);

  void unloadCurrentScene();

  std::unordered_map<std::string, SceneFactory> scenes;

  std::unique_ptr<Scene> currentScene;

  std::string currentSceneName;
  std::string queuedScene;

  bool transitioning = false;
  bool fadingOut = true;

  float transitionTimer = 0.f;
  float transitionDuration = 0.5f;
};