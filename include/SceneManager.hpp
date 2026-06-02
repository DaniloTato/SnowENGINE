#pragma once

#include "MapLifecycle.hpp"
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

  struct SceneInstance {
    std::string name;
    std::unique_ptr<Scene> scene;
    bool transitioning = false;
    std::string queuedScene;
    bool fadingOut = true;
    float timer = 0.f;
    float duration = 0.5f;

    SceneInstance(std::string sceneName, std::unique_ptr<Scene> scenePtr)
        : name(std::move(sceneName)), scene(std::move(scenePtr)) {}
  };

  bool openScene(const std::string &slot, const std::string &sceneName,
                 const Scene::Context &ctx);

  bool transitionScene(const std::string &slot, const std::string &sceneName);
  void closeScene(const std::string &slot);

  Scene *getScene(const std::string &slot);

  void update(const GeneralContext &ctx, const Scene::Context &sceneCtx);

  bool reloadScene(const std::string &slot);

private:
  std::unordered_map<std::string, SceneInstance> activeScenes;
  MapLifecycle<SceneInstance> lifecycle;
};