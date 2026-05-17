/*SFML dependency*/
#include <SFML/Graphics.hpp>

/*Engine Objects*/
#include "BulletManager.hpp"
#include "CollectableManager.hpp"
#include "DialogueManager.hpp"
#include "EnemyManager.hpp"
#include "GameLoader.hpp"
#include "GameObject.hpp"
#include "GameState.hpp"
#include "InputManager.hpp"
#include "LevelManager.hpp"
#include "Renderizer.hpp"
#include "SceneManager.hpp"
#include "Terminal.hpp"
#include "WindowLifecycleListener.hpp"

/*Helpers*/
#include "Helpers.hpp"

/*SceneBuilders*/
#include "Helpers.hpp"

#include "Constants.hpp"

#include <cstddef>
#include <cstdlib>
#include <ctime>

using namespace Helper;

int main() {
  InputManager &inputManager = InputManager::getInstance();
  SceneManager &sceneManager = SceneManager::getInstance();
  WindowManager &windowManager = WindowManager::getInstance();
  GameState &gameState = GameState::getInstance();

  WindowID mainWindow = windowManager.create(
      WindowManager::Set::MAIN, Constants::SCREEN_WIDTH,
      Constants::SCREEN_HEIGHT, Constants::MAIN_WINDOW_NAME);
  WindowLifecycleListener lifecycleListener;

  windowManager.subscribe(mainWindow, &inputManager);
  windowManager.subscribe(mainWindow, &lifecycleListener);

  inputManager.loadBindingsFromJsonFile(
      (Helper::getPath("config/control_config.json")));

  GameLoader loader;
  loader.loadGameData(Helper::getPath("config"));

  sf::Clock clock;

  while (windowManager.isMainWindowAlive()) {

    inputManager.update();
    windowManager.pollEvents();

    const std::unordered_map<WindowID, WindowManager::WindowEntry> &windows =
        windowManager.getAll();

    sceneManager.update();
    gameState.updateDt();

    if (!sceneManager.isTransitioning()) {
      LevelManager::getInstance().applyQueuedTileChanges();
      DialogueManager::getInstance().applyQueues();
      BulletManager::getInstance().applyQueues();
      EnemyManager::getInstance().applyQueues();
      CollectableManager::getInstance().applyQueues();
      BulletManager::getInstance().update();
      Terminal::destroyKilledTerminals();
      windowManager.applyDestroyQueue();
    }

    for (const auto &[id, entry] : windows) {

      if (entry.set == WindowManager::Set::MAIN) {
        windowManager.clearWindow(
            id, LevelManager::getInstance().getBackgroundColor());
      } else {
        windowManager.clearWindow(id);
      }
    }

    // IDEA: Group objects by domain like windows and only call a certain domain
    // to update.
    for (GameObject *gameObject : GameObject::getGameObjects()) {
      if (gameObject && !gameObject->isUpdateDomainPaused()) {
        gameObject->update(gameState.getGeneralContext());
      }
    }

    Renderizer::renderAll();

    for (const auto &[id, entry] : windows) {
      windowManager.checkRenderFlag(id);
    }

    sf::sleep(sf::milliseconds(1));
  }

  return 0;
}