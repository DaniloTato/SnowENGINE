/*SFML dependency*/
#include <SFML/Graphics.hpp>

/*Engine Objects*/
#include "BulletManager.hpp"
#include "CollectableManager.hpp"
#include "DialogueManager.hpp"
#include "EnemyManager.hpp"
#include "Engine.hpp"
#include "GameLoader.hpp"
#include "GameState.hpp"
#include "InputManager.hpp"
#include "LevelManager.hpp"
#include "Renderizer.hpp"
#include "SceneManager.hpp"
#include "WindowLifecycleListener.hpp"

/*Helpers*/
#include "Helpers.hpp"

/*SceneBuilders*/
#include "Helpers.hpp"

#include "Constants.hpp"

#include <cstddef>
#include <cstdlib>
#include <ctime>

int main() {

  Engine engine;

  InputManager &inputManager = InputManager::getInstance();
  GameState &gameState = GameState::getInstance();

  SceneManager &sceneManager = engine.getSceneManager();
  LevelManager &levelManager = engine.getLevelManager();
  WindowManager &windowManager = engine.getWindowManager();
  DialogueManager &dialogueManager = engine.getDialogueManager();

  WindowID mainWindow = windowManager.create(
      WindowManager::Set::MAIN, Constants::SCREEN_WIDTH,
      Constants::SCREEN_HEIGHT, Constants::MAIN_WINDOW_NAME);
  windowManager.applyQueues();
  WindowLifecycleListener lifecycleListener(windowManager);

  windowManager.subscribe(mainWindow, &inputManager);
  windowManager.subscribe(mainWindow, &lifecycleListener);

  inputManager.loadBindingsFromJsonFile(
      (Helper::getPath("config/control_config.json")));

  GameLoader loader;
  loader.loadGameData(Helper::getPath("config"), engine, mainWindow);

  sf::Clock clock;

  while (windowManager.isMainWindowAlive()) {

    inputManager.update();
    windowManager.pollEvents();

    const std::unordered_map<WindowID, WindowManager::WindowEntry> &windows =
        windowManager.getAll();

    gameState.updateDt();

    sceneManager.update(
        gameState.getGeneralContext(),
        Scene::Context{.engine = &engine, .mainWindow = mainWindow});

    // now we dont block instances of managers when transitioning scenes.
    levelManager.applyQueuedTileChanges();
    dialogueManager.applyQueues();
    BulletManager::getInstance().applyQueues();
    EnemyManager::getInstance().applyQueues();
    CollectableManager::getInstance().applyQueues();
    BulletManager::getInstance().update();

    for (const auto &[id, entry] : windows) {

      if (entry.set == WindowManager::Set::MAIN) {
        windowManager.clearWindow(id, levelManager.getBackgroundColor());
      } else {
        windowManager.clearWindow(id);
      }
    }

    sceneManager.render(engine.getRenderSystem());

    for (const auto &[id, entry] : windows) {
      windowManager.checkRenderFlag(id);
    }

    // Position is to avoid fetching an already deleted window on last frame.
    windowManager.applyQueues();

    sf::sleep(sf::milliseconds(1));
  }

  return 0;
}