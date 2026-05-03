#include "SceneManager.hpp"
#include "BulletManager.hpp"
#include "CollectableManager.hpp"
#include "Constants.hpp"
#include "DialogueManager.hpp"
#include "EnemyManager.hpp"
#include "GameObject.hpp"
#include "GameState.hpp"
#include "LevelManager.hpp"
#include "ParticleManager.hpp"

SceneManager &SceneManager::getInstance() {
  static SceneManager instance;
  return instance;
}

void SceneManager::registerScene(const std::string &name,
                                 const SceneSetupFn &setup) {
  scenes[name] = setup;
}

bool SceneManager::loadScene(const std::string &name) {
  if (transitioning) {
    return true;
  }

  if (scenes.find(name) == scenes.end()) {
    return false;
  }

  beginTransition(name);

  return true;
}

void SceneManager::reloadCurrentScene() { loadScene(currentScene); }

void SceneManager::beginTransition(const std::string &nextScene) {
  transitioning = true;
  fadingOut = true;
  transitionTimer = 0.f;
  queuedScene = nextScene;

  initFadeOverlay();
}

void SceneManager::unloadCurrentScene() {

  BulletManager::getInstance().onSceneUnload();
  EnemyManager::getInstance().onSceneUnload();
  ParticleManager::getInstance().onSceneUnload();
  DialogueManager::getInstance().onSceneUnload();
  LevelManager::getInstance().onSceneUnload();
  CollectableManager::getInstance().onSceneUnload();

  GameState::getInstance().clearCameras();

  GameObject::destroySceneObjects();
}

void SceneManager::update() {
  if (!transitioning)
    return;

  float dt = GameState::getInstance().dt();
  transitionTimer += dt;

  float t = transitionTimer / transitionDuration;
  if (t > 1.f)
    t = 1.f;

  if (fadingOut) {
    auto alpha = static_cast<sf::Uint8>(255.f * t);
    fadeOverlay->renderizer.setColor(sf::Color(0, 0, 0, alpha));

    if (t >= 1.f) {
      unloadCurrentScene();
      scenes[queuedScene]();
      currentScene = queuedScene;

      fadingOut = false;
      transitionTimer = 0.f;
    }
  } else {
    auto alpha = static_cast<sf::Uint8>(255.f * (1.f - t));
    fadeOverlay->renderizer.setColor(sf::Color(0, 0, 0, alpha));

    if (t >= 1.f) {
      fadeOverlay->renderizer.setColor(sf::Color(0, 0, 0, 0));
      transitioning = false;
    }
  }
}

bool SceneManager::isTransitioning() { return transitioning; }

void SceneManager::initFadeOverlay() {
  if (fadeOverlay)
    return;

  sf::Texture dummyTexture;

  RenderizerParameters params{.window = WindowManager::getInstance().getMain(),
                              .texture = &dummyTexture,
                              .camera = nullptr,
                              .layer = Constants::OVERLAY_LAYER,
                              .parallax = 1.f,
                              .registerAsRectShape = true};

  fadeOverlay = new RenderableObject(params);
  fadeOverlay->renderizer.setRect(
      {0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT}, 1);
  fadeOverlay->makePersistentAcrossScenes();
  fadeOverlay->renderizer.setColor(sf::Color(0, 0, 0, 0));
}

SceneManager::SceneNameList SceneManager::getRegisteredScenes() const {
  SceneNameList result;
  result.reserve(scenes.size());

  for (const auto &[name, _] : scenes) {
    result.push_back(name);
  }

  return result;
}