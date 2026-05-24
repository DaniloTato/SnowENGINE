#include "tilePickerCreationScript.hpp"
#include "Constants.hpp"
#include "EnemyManager.hpp"
#include "InputManager.hpp"
#include "LevelManager.hpp"
#include "TilePicker.hpp"
#include <iostream>

#include "RegistryMacros.hpp"
#include "ScriptRegistry.hpp" // IWYU pragma: keep
#include "Scripter.hpp"

namespace Scripts {

namespace {
struct tilePickerCreationScriptState {
  std::shared_ptr<TilePicker> picker;
  sf::IntRect selectedTileRect;
  int selectedActiveLayer = 0;
  std::string selectedEnemyId;
  PickerMode mode;
};
} // namespace

void tilePickerCreationScript(ScriptRunner &scriptRunner,
                              const GeneralContext &ctx) {

  auto &state = scriptRunner.scripter.getState<tilePickerCreationScriptState>(
      "tilePickerCreationScript");

  InputManager &inputManager = InputManager::getInstance();
  LevelManager &levelManager = ctx.engine->getLevelManager();
  WindowManager &windowManager = ctx.engine->getWindowManager();

  if (!state.picker) {
    state.picker = std::make_shared<TilePicker>(
        levelManager.getTilesheet(), Constants::TILE_SIZE,
        ctx.engine->getWindowManager(), ctx.engine->getLevelManager());
    state.picker->setLayers(&levelManager.layers);
  }

  const WindowID main = windowManager.getMain();
  const CameraID mainCamera = ctx.mainCamera;

  if (inputManager.isJustPressed("tilePicker")) {
    if (!state.picker->isOpen()) {
      state.picker->open();
      windowManager.subscribe(state.picker->getWindow(),
                              &InputManager::getInstance());
      windowManager.subscribe(state.picker->getWindow(), state.picker.get());
    } else {
      state.picker->close(ctx.engine->getWindowManager());
      // HORRIBLE BUG WAITING TO HAPPEN LOL
      ctx.engine->getLevelManager().reloadAllLayers();
    }
  }

  if (state.picker && state.picker->isOpen()) {

    state.picker->update();
    state.picker->draw();

    PickerSelection sel = state.picker->getSelection();
    state.selectedTileRect = sel.tileRect;
    state.selectedEnemyId = sel.enemyId;
    state.selectedActiveLayer = sel.activeLayer;
    state.mode = sel.mode;
  }

  try {

    if (inputManager.isJustPressed("spawnEnemy")) {
      EnemyManager::getInstance().queueCreateEnemy(
          state.selectedEnemyId,
          ctx.cameraManager->getCamera(mainCamera)
              ->screenToWorld(
                  {static_cast<float>(inputManager.getMousePosition(main).x),
                   static_cast<float>(inputManager.getMousePosition(main).y)},
                  1.f));
    }

    if (state.mode == PickerMode::Tiles) {
      if (inputManager.isPressed("createTile")) {
        sf::Vector2f mousePosToTilePos =
            ctx.cameraManager->getCamera(mainCamera)
                ->screenToWorld(
                    {static_cast<float>(inputManager.getMousePosition(main).x),
                     static_cast<float>(inputManager.getMousePosition(main).y)},
                    levelManager.getLayerInfo(state.selectedActiveLayer)
                        .paralax);

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

            levelManager.queueCreateTile(state.selectedActiveLayer,
                                         baseTileX + x, baseTileY + y, subRect);
          }
        }
      }
    } else if (state.mode == PickerMode::Enemies) {
      if (inputManager.isJustPressed("createTile")) {
        sf::Vector2f mousePosToEnemyPos =
            ctx.cameraManager->getCamera(mainCamera)
                ->screenToWorld(
                    {static_cast<float>(inputManager.getMousePosition(main).x),
                     static_cast<float>(inputManager.getMousePosition(main).y)},
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
          ctx.cameraManager->getCamera(mainCamera)
              ->screenToWorld(
                  {static_cast<float>(inputManager.getMousePosition(main).x),
                   static_cast<float>(inputManager.getMousePosition(main).y)},
                  levelManager.getLayerInfo(state.selectedActiveLayer).paralax);

      levelManager.queueDeleteTile(
          state.selectedActiveLayer,
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
          ctx.cameraManager->getCamera(mainCamera)
              ->screenToWorld(
                  {static_cast<float>(inputManager.getMousePosition(main).x),
                   static_cast<float>(inputManager.getMousePosition(main).y)},
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

REGISTER_SCRIPT("tilePickerCreationScript", Scripts::tilePickerCreationScript,
                ScriptRunner);