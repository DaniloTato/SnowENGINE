#include "tilePickerCreationScript.hpp"
#include "Constants.hpp"
#include "EnemyManager.hpp"
#include "GameState.hpp"
#include "InputManager.hpp"
#include "LevelManager.hpp"
#include "TilePicker.hpp"
#include <iostream>

namespace Scripts {

namespace {
struct tilePickerCreationScriptState {
  std::shared_ptr<TilePicker> picker;
  sf::IntRect selectedTileRect;
  std::string selectedEnemyId;
  PickerMode mode;
};
} // namespace

void tilePickerCreationScript(ScriptRunner &scriptRunner,
                              const GeneralContext &ctx) {

  auto &state = scriptRunner.scripter.getState<tilePickerCreationScriptState>(
      "tilePickerCreationScript");

  InputManager &inputManager = InputManager::getInstance();
  LevelManager &levelManager = LevelManager::getInstance();
  WindowManager &windowManager = WindowManager::getInstance();

  if (!state.picker) {
    state.picker = std::make_shared<TilePicker>(levelManager.getTilesheet(),
                                                Constants::TILE_SIZE);
    state.picker->setLayers(&levelManager.layers);
  }

  const WindowManager::WindowID window = windowManager.getMain();

  if (inputManager.isJustPressed("tilePicker")) {
    if (!state.picker->isOpen()) {
      state.picker->open();
    } else {
      state.picker->close();
    }
  }

  if (state.picker && state.picker->isOpen()) {
    sf::Event event;
    WindowManager::WindowID window = state.picker->getWindow();
    while (windowManager.pollEventOnWindow(window, event)) {
      inputManager.handleEvent(window, event);
      state.picker->handleEvent(window, event);
    }

    state.picker->update();
    state.picker->draw();

    PickerSelection sel = state.picker->getSelection();
    state.selectedTileRect = sel.tileRect;
    state.selectedEnemyId = sel.enemyId;
    state.mode = sel.mode;
  }

  try {

    if (inputManager.isJustPressed("spawnEnemy")) {
      EnemyManager::getInstance().queueCreateEnemy(
          state.selectedEnemyId,
          GameState::getInstance().getMainCamera()->screenToWorld(
              {static_cast<float>(inputManager.getMousePosition(window).x),
               static_cast<float>(inputManager.getMousePosition(window).y)},
              1.f));
    }

    if (state.mode == PickerMode::Tiles) {
      if (inputManager.isPressed("createTile")) {
        sf::Vector2f mousePosToTilePos =
            GameState::getInstance().getMainCamera()->screenToWorld(
                {static_cast<float>(inputManager.getMousePosition(window).x),
                 static_cast<float>(inputManager.getMousePosition(window).y)},
                levelManager.getLayerInfo(levelManager.activeLayer).paralax);

        sf::IntRect &selRect = state.selectedTileRect;
        const int tileSize = Constants::TILE_SIZE;
        int tilesWide = selRect.width / tileSize;
        int tilesHigh = selRect.height / tileSize;
        int baseTileX = static_cast<int>(mousePosToTilePos.x) / tileSize;
        int baseTileY = static_cast<int>(mousePosToTilePos.y) / tileSize;

        for (int y = 0; y < tilesHigh; y++) {
          for (int x = 0; x < tilesWide; x++) {
            sf::IntRect subRect(selRect.left + x * tileSize,
                                selRect.top + y * tileSize, tileSize, tileSize);

            levelManager.queueCreateTile(
                window, GameState::getInstance().getMainCamera(),
                levelManager.activeLayer, baseTileX + x, baseTileY + y,
                subRect);
          }
        }
      }
    } else if (state.mode == PickerMode::Enemies) {
      if (inputManager.isJustPressed("createTile")) {
        sf::Vector2f mousePosToEnemyPos =
            GameState::getInstance().getMainCamera()->screenToWorld(
                {static_cast<float>(inputManager.getMousePosition(window).x),
                 static_cast<float>(inputManager.getMousePosition(window).y)},
                1.f);
        sf::Vector2i EnemyPosInt = {static_cast<int>(mousePosToEnemyPos.x),
                                    static_cast<int>(mousePosToEnemyPos.y)};

        EnemyPosInt /= Constants::TILE_SIZE;
        EnemyPosInt *= Constants::TILE_SIZE;

        EnemyManager::getInstance().queueCreateEnemy(
            state.selectedEnemyId, {static_cast<float>(EnemyPosInt.x),
                                    static_cast<float>(EnemyPosInt.y)});
      }
    }

    if (inputManager.isPressed("deleteTile")) {
      sf::Vector2f mousePosToTilePos =
          GameState::getInstance().getMainCamera()->screenToWorld(
              {static_cast<float>(inputManager.getMousePosition(window).x),
               static_cast<float>(inputManager.getMousePosition(window).y)},
              levelManager.getLayerInfo(levelManager.activeLayer).paralax);

      levelManager.queueDeleteTile(
          levelManager.activeLayer,
          static_cast<int>(mousePosToTilePos.x) / Constants::TILE_SIZE,
          static_cast<int>(mousePosToTilePos.y) / Constants::TILE_SIZE);
    }

    if (inputManager.isJustPressed("saveLevel")) {
      std::cout << "Saving level on: " << levelManager.getLoadedLevelPath()
                << "\n";
      levelManager.saveLevel(levelManager.getLoadedLevelPath());
    }

    if (inputManager.isMouseJustPressed(MouseButton::Left)) {
      sf::Vector2f mousePosInWorld =
          GameState::getInstance().getMainCamera()->screenToWorld(
              {static_cast<float>(inputManager.getMousePosition(window).x),
               static_cast<float>(inputManager.getMousePosition(window).y)},
              1.f);
      std::cout << "(" << mousePosInWorld.x << "," << mousePosInWorld.y << ")"
                << " "
                << "("
                << static_cast<int>(mousePosInWorld.x) / Constants::TILE_SIZE
                << ","
                << static_cast<int>(mousePosInWorld.y) / Constants::TILE_SIZE
                << ")\n";
    }
  } catch (LevelManager::Error err) {
    std::cerr << err.message << "\n";
  }
}
} // namespace Scripts