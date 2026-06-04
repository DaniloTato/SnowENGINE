#include "SceneManager.hpp"

#include "GameState.hpp"
#include "SceneBuilderRegistry.hpp"

bool SceneManager::openScene(const std::string &slot,
                             const std::string &sceneName,
                             const Scene::Context &ctx) {
  if (activeScenes.contains(slot)) {
    return false;
  }
  auto factory = SceneBuilderRegistry::get(sceneName);
  auto scene = factory();
  scene->setup(ctx);

  lifecycle.queueCreate(slot, SceneInstance(sceneName, std::move(scene)));

  return true;
}

bool SceneManager::transitionScene(const std::string &slot,
                                   const std::string &sceneName) {
  auto it = activeScenes.find(slot);

  if (it == activeScenes.end()) {
    return false;
  }

  it->second.transitioning = true;
  it->second.fadingOut = true;
  it->second.timer = 0.f;
  it->second.queuedScene = sceneName;

  return true;
}

void SceneManager::closeScene(const std::string &slot) {
  lifecycle.queueDestroy(slot);
}

Scene *SceneManager::getScene(const std::string &slot) {
  auto it = activeScenes.find(slot);

  if (it == activeScenes.end()) {
    return nullptr;
  }

  return it->second.scene.get();
}

void SceneManager::update(const GeneralContext &ctx,
                          const Scene::Context &sceneCtx) {
  for (auto &[_, instance] : activeScenes) {
    if (instance.scene) {
      instance.scene->update(ctx);
    }
  }

  float dt = GameState::getInstance().dt();

  for (auto &[_, instance] : activeScenes) {

    if (!instance.transitioning) {
      continue;
    }

    instance.timer += dt;

    float t = instance.timer / instance.duration;
    t = std::min(t, 1.f);

    if (instance.fadingOut) {

      if (t >= 1.f) {

        auto factory = SceneBuilderRegistry::get(instance.queuedScene);

        auto replacement = factory();

        replacement->setup(sceneCtx);

        instance.scene = std::move(replacement);
        instance.name = instance.queuedScene;

        instance.fadingOut = false;
        instance.timer = 0.f;
      }

    } else {

      if (t >= 1.f) {
        instance.transitioning = false;
      }
    }
  }

  lifecycle.apply(activeScenes);
}

bool SceneManager::reloadScene(const std::string &slot) {
  auto it = activeScenes.find(slot);

  if (it == activeScenes.end()) {
    return false;
  }

  return transitionScene(slot, it->second.name);
}

void SceneManager::render(RenderSystem &renderer) {
  for (auto &[name, scene] : activeScenes) {
    scene.scene->render(renderer);
  }
}