#include "SceneManager.hpp"

#include "Engine.hpp"
#include "GameState.hpp"
#include "Scene.hpp"

void SceneManager::registerScene(const std::string &name,
                                 SceneFactory factory) {

  scenes[name] = std::move(factory);
}

bool SceneManager::loadScene(const std::string &name, Engine &engine) {

  if (transitioning) {
    return true;
  }

  if (!scenes.contains(name)) {
    return false;
  }

  beginTransition(name, engine);

  return true;
}

void SceneManager::reloadCurrentScene(Engine &engine) {

  loadScene(currentSceneName, engine);
}

void SceneManager::beginTransition(const std::string &nextScene,
                                   Engine &engine) {

  transitioning = true;
  fadingOut = true;

  transitionTimer = 0.f;

  queuedScene = nextScene;
}

void SceneManager::unloadCurrentScene() { currentScene.reset(); }

void SceneManager::update(const GeneralContext &ctx,
                          const Scene::Context &sceneContext) {

  if (currentScene) {
    currentScene->update(ctx);
  }

  if (!transitioning) {
    return;
  }

  float dt = GameState::getInstance().dt();

  transitionTimer += dt;

  float t = transitionTimer / transitionDuration;

  t = std::min(t, 1.f);

  if (fadingOut) {

    if (t >= 1.f) {
      unloadCurrentScene();
      currentScene = scenes[queuedScene]();
      currentScene->setup(sceneContext);
      currentSceneName = queuedScene;
      fadingOut = false;
      transitionTimer = 0.f;
    }
  } else {
    if (t >= 1.f) {
      transitioning = false;
    }
  }
}

Scene *SceneManager::getCurrentScene() { return currentScene.get(); }

bool SceneManager::isTransitioning() { return transitioning; }

SceneManager::SceneNameList SceneManager::getRegisteredScenes() const {
  SceneNameList result;
  result.reserve(scenes.size());
  for (const auto &[name, _] : scenes) {
    result.push_back(name);
  }
  return result;
}