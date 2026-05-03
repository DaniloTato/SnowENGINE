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

  WindowManager::WindowID window = WindowManager::getInstance().getMain();

  try {

    if (inputManager.isJustPressed("tilePicker")) {
      TilePicker picker(levelManager.getTilesheet(), Constants::TILE_SIZE);

      PickerSelection selection =
          picker.open(levelManager.layers, levelManager.activeLayer);
      state.selectedTileRect = selection.tileRect;
      state.selectedEnemyId = selection.enemyId;
      state.mode = selection.mode;
      levelManager.reloadAllLayers(window,
                                   GameState::getInstance().getMainCamera());
    }

    if (inputManager.isJustPressed("spawnEnemy")) {
      EnemyManager::getInstance().queueCreateEnemy(
          state.selectedEnemyId,
          GameState::getInstance().getMainCamera()->screenToWorld(
              {static_cast<float>(inputManager.getMousePosition().x),
               static_cast<float>(inputManager.getMousePosition().y)},
              1.f));
    }

    if (state.mode == PickerMode::Tiles) {
      if (inputManager.isPressed("createTile")) {
        sf::Vector2f mousePosToTilePos =
            GameState::getInstance().getMainCamera()->screenToWorld(
                {static_cast<float>(inputManager.getMousePosition().x),
                 static_cast<float>(inputManager.getMousePosition().y)},
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
                {static_cast<float>(inputManager.getMousePosition().x),
                 static_cast<float>(inputManager.getMousePosition().y)},
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
              {static_cast<float>(inputManager.getMousePosition().x),
               static_cast<float>(inputManager.getMousePosition().y)},
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
              {static_cast<float>(inputManager.getMousePosition().x),
               static_cast<float>(inputManager.getMousePosition().y)},
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