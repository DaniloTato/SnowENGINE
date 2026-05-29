#include "SceneManager.hpp"
#include "Constants.hpp"
#include "GameObject.hpp"
#include "GameState.hpp"
#include "SceneContext.hpp"

void SceneManager::registerScene(const std::string &name,
                                 const SceneSetupFn &setup) {
  scenes[name] = setup;
}

bool SceneManager::loadScene(const std::string &name, Engine &engine) {
  if (transitioning) {
    return true;
  }

  if (scenes.find(name) == scenes.end()) {
    return false;
  }

  beginTransition(name, engine);

  return true;
}

void SceneManager::reloadCurrentScene(Engine &engine) {
  loadScene(currentScene, engine);
}

void SceneManager::beginTransition(const std::string &nextScene,
                                   Engine &engine) {
  transitioning = true;
  fadingOut = true;
  transitionTimer = 0.f;
  queuedScene = nextScene;

  initFadeOverlay(engine);
}

void SceneManager::unloadCurrentScene() {

  SceneAware::notifySceneUnload();
  GameObject::destroySceneObjects();
}

void SceneManager::update(SceneBuilder::SceneContext ctx) {
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
      scenes[queuedScene](ctx);
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

void SceneManager::initFadeOverlay(Engine &engine) {
  if (fadeOverlay)
    return;

  sf::Texture dummyTexture;

  RenderizerParameters params{.engine = engine,
                              .window = engine.getWindowManager().getMain(),
                              .texture = &dummyTexture,
                              .camera = CameraID(),
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