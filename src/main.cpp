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

/*Helpers*/
#include "Helpers.hpp"

/*SceneBuilders*/
#include "Helpers.hpp"

#include <cstddef>
#include <cstdlib>
#include <ctime>

#include <cstdio>

using namespace Helper;

int main() {
  InputManager &inputManager = InputManager::getInstance();
  SceneManager &sceneManager = SceneManager::getInstance();
  GameState &gameState = GameState::getInstance();

  inputManager.loadBindingsFromJsonFile(
      (Helper::getPath("config/control_config.json")));

  GameState::getInstance().createCamera(CameraTypes::UI, {WindowTypes::MAIN});
  GameState::getInstance().getUiCamera()->makePersistentAcrossScenes();

  GameLoader loader;
  loader.loadGameData(Helper::getPath("config"));

  sf::Clock clock;

  while (gameState.getMainWindow() && gameState.getMainWindow()->isOpen()) {

    sf::Event event;
    inputManager.update();

    std::vector<GameWindow> &windows = gameState.getWindows();
    for (WindowTypes i = WindowTypes::MAIN; i != WindowTypes::COUNT;
         i = static_cast<WindowTypes>(static_cast<int>(i) + 1)) {

      auto windowType = static_cast<size_t>(i);
      if (!windows[windowType].getWindow() || i == WindowTypes::TERMINAL)
        continue;

      while (windows[windowType].getWindow()->pollEvent(event)) {
        if (i == WindowTypes::MAIN) {
          inputManager.handleEvent(event);
        }

        if (event.type == sf::Event::Closed) {
          GameState::getInstance().removeWindow(i);
          break;
        }

        if (event.type == sf::Event::LostFocus) {
          gameState.pauseWindow(i);
        }

        if (event.type == sf::Event::GainedFocus) {
          gameState.resumeWindow(i);
        }
      }
    }

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
    }

    for (WindowTypes i = WindowTypes::MAIN; i != WindowTypes::COUNT;
         i = static_cast<WindowTypes>(static_cast<int>(i) + 1)) {

      auto windowType = static_cast<size_t>(i);
      if (!windows[windowType].getWindow())
        continue;

      if (i == WindowTypes::MAIN) {
        windows[windowType].getWindow()->clear(
            LevelManager::getInstance().getBackgroundColor());
      } else {
        windows[windowType].getWindow()->clear();
      }
    }

    for (GameObject *gameObject : GameObject::getGameObjects()) {
      if (gameObject && !gameObject->isUpdateDomainPaused()) {
        gameObject->update(gameState.getGeneralContext());
      }
    }

    Renderizer::renderAll();

    for (auto &gameWindow : gameState.getWindows()) {
      if (!gameWindow.getWindow())
        continue;

      if (gameWindow.renderFlag()) {
        gameWindow.getWindow()->display();
        gameWindow.markAsRendered();
      }
    }

    sf::sleep(sf::milliseconds(1));
  }

  return 0;
}