#include "LevelManager.hpp"
#include "ColorPalette.hpp"
#include "Constants.hpp"
#include "Helpers.hpp"
#include "RenderableObject.hpp"
#include "SFML/Graphics/Rect.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

RenderableObject *LevelManager::TileFactory::create(
    const TileInfo &tile, const RenderizerParameters &params, int tileSize) {
  auto *obj = new RenderableObject(params);
  obj->position = {float(tile.x * tileSize + 1), float(tile.y * tileSize + 1)};

  obj->renderizer.setRect(tile.textureRect, 1);

  return obj;
}

void LevelManager::TileFactory::destroy(RenderableObject *&obj) {
  if (obj) {
    GameObject::destroy(obj);
    obj = nullptr;
  }
}

using json = nlohmann::json;

LevelManager::LevelManager()
    : isLevelLoaded(false), backgroundColor(ColorPalette::Black) {}

sf::Color &LevelManager::getBackgroundColor() { return backgroundColor; }

void LevelManager::loadLevel(const std::string &path) {

  layers.clear();

  std::ifstream file(path);
  if (!file.is_open()) {
    throw LevelManager::Error("[LevelManager] Failed to open level file: " +
                              path);
  }

  loadedLevelpath = path;

  json data = json::parse(file);

  tilesetPath = data["tileset"].get<std::string>();

  if (!tilesheet.loadFromFile(Helper::getPath(tilesetPath))) {
    std::cerr << "[LevelManager] Failed to open tileset texture path: " +
                     tilesetPath + ". Check the path closely for errors.\n";
    return;
  }

  int tileSize = data["tile_size"];

  auto &jsonLayers = data["layers"];
  layers.resize(jsonLayers.size());

  int width = 1000;
  int height = 1000;
  levelLayout.assign(height, std::vector<int>(width, 0));

  for (size_t i = 0; i < jsonLayers.size(); i++) {
    layers[i].name = jsonLayers[i]["name"].get<std::string>();
    layers[i].paralax = jsonLayers[i]["parallax"].get<float>();
    loadLayer(i, jsonLayers[i], tileSize);

    // put secret layer above others
    if (layers[i].name == "secret") {
      for (auto &tile : layers[i].tiles) {
        tile.object->renderizer.setLayer(-1);
      }
    }
  }

  isLevelLoaded = true;
}

void LevelManager::deleteLayerObjects(int layerNo) {
  LayerInfo layer = layers[layerNo];

  for (auto &tile : layer.tiles) {
    TileFactory::destroy(tile.object);
  }
}

void LevelManager::loadLayer(size_t layerNo, const json &layerJSON,
                             int tileSize) {

  auto &tileList = layers[layerNo].tiles;
  tileList.clear();

  const auto &jsonTiles = layerJSON["tiles"];

  for (const auto &t : jsonTiles) {
    TileInfo info;
    info.x = t["x"].get<int>();
    info.y = t["y"].get<int>();

    size_t unsignedInfoX = info.x;
    size_t unsignedInfoY = info.y;

    info.textureRect = sf::IntRect(t["tex_x"].get<int>(), t["tex_y"].get<int>(),
                                   tileSize, tileSize);

    info.object = TileFactory::create(info, makeRenderParams(layerNo),
                                      Constants::TILE_SIZE);

    tileList.push_back(info);

    if (layerNo == 0 && unsignedInfoY < levelLayout.size() &&
        unsignedInfoX < levelLayout[0].size())
      levelLayout[unsignedInfoY][unsignedInfoX] = 1;
  }
}

void LevelManager::reloadAllLayers() {
  ensureLevelLoaded();
  for (size_t i = 0; i < layers.size(); ++i)
    reloadLayer(i);
}

void LevelManager::reloadLayer(size_t layerNo) {

  ensureLevelLoaded();

  auto &tiles = layers[layerNo].tiles;

  for (auto &t : tiles) {
    TileFactory::destroy(t.object);
    t.object =
        TileFactory::create(t, makeRenderParams(layerNo), Constants::TILE_SIZE);
  }
}

void LevelManager::createTile(int layerNo, int x, int y, sf::IntRect rect) {
  ensureLevelLoaded();

  size_t unsignedX = x;
  size_t unsignedY = y;

  if (layerNo < 0 || layerNo >= (int)layers.size()) {
    std::cerr << "[LevelManager][createTile] ERROR: Invalid layerNo: "
              << layerNo << "\n";
    return;
  }

  if (layerNo == 0) {
    if (x < 0 || y < 0) {
      std::cerr
          << "[LevelManager][createTile] ERROR: Negative tile coordinates: ("
          << x << ", " << y << ")\n";
      return;
    }

    if ((unsignedY >= levelLayout.size() ||
         unsignedX >= levelLayout[y].size())) {
      std::cerr << "[LevelManager][createTile] ERROR: Tile coordinates out of "
                   "levelLayout bounds: ("
                << x << ", " << y << ")\n";
      return;
    }
  }

  std::vector<TileInfo> &tiles = layers[layerNo].tiles;

  for (auto &t : tiles) {
    if (t.x == x && t.y == y) {
      TileFactory::destroy(t.object);

      if (layerNo == 0) {
        levelLayout[unsignedY][unsignedX] = 1;
      }

      t.textureRect = rect;

      t.object = TileFactory::create(t, makeRenderParams(layerNo),
                                     Constants::TILE_SIZE);

      return;
    }
  }

  TileInfo info;
  info.x = x;
  info.y = y;
  info.textureRect = rect;

  if (layerNo == 0) {
    levelLayout[unsignedY][unsignedX] = 1;
  }

  info.object = TileFactory::create(info, makeRenderParams(layerNo),
                                    Constants::TILE_SIZE);

  tiles.push_back(info);
}

void LevelManager::deleteTile(int layerNo, int x, int y) {

  ensureLevelLoaded();

  size_t unsignedX = x;
  size_t unsignedY = y;

  if (layerNo == 0 && (x < 0 || y < 0))
    return;

  std::vector<TileInfo> &tiles = layers[layerNo].tiles;

  std::erase_if(tiles, [&](TileInfo &t) {
    if (t.x == x && t.y == y) {
      TileFactory::destroy(t.object);
      return true;
    }
    return false;
  });

  if (layerNo == 0 && unsignedY < levelLayout.size() &&
      unsignedX < levelLayout[y].size()) {
    levelLayout[unsignedY][unsignedX] = 0;
  }
}

void LevelManager::saveLevel(const std::string &path) {

  ensureLevelLoaded();

  json data;

  data["tile_size"] = Constants::TILE_SIZE;
  data["tileset"] = tilesetPath;

  data["layers"] = json::array();

  for (const auto &layer : layers) {
    json layerJson;
    layerJson["name"] = layer.name;
    layerJson["parallax"] = layer.paralax;

    layerJson["tiles"] = json::array();
    for (const auto &tile : layer.tiles) {
      layerJson["tiles"].push_back({{"x", tile.x},
                                    {"y", tile.y},
                                    {"tex_x", tile.textureRect.left},
                                    {"tex_y", tile.textureRect.top}});
    }

    data["layers"].push_back(layerJson);
  }

  std::ofstream file(path);
  if (!file.is_open()) {
    std::cerr << "[LevelManager] Failed to open " << path << " for writing!\n";
    return;
  }

  file << data.dump(4);
  file.close();
}

LevelManager::LevelLayout2D &LevelManager::getLevelLayout() const {
  ensureLevelLoaded();
  return levelLayout;
}

sf::Texture &LevelManager::getTilesheet() {
  ensureLevelLoaded();
  return tilesheet;
}

const LayerInfo LevelManager::getLayerInfo(int layerNo) const {
  ensureLevelLoaded();
  if (layerNo >= static_cast<int>(layers.size())) {
    throw("[LevelManager] Accessing non existing layer\n");
  }

  return layers[layerNo];
}

void LevelManager::queueCreateTile(int layer, int x, int y,
                                   const sf::IntRect &rect) {
  ensureLevelLoaded();
  createQueue.push_back(
      TileCreationRequest{.layer = layer, .x = x, .y = y, .rect = rect});
}

void LevelManager::queueDeleteTile(int layer, int x, int y) {
  ensureLevelLoaded();
  deleteQueue.push_back({layer, x, y});
}

void LevelManager::applyQueuedTileChanges() {
  for (const auto &delReq : deleteQueue) {
    deleteTile(delReq.layer, delReq.x, delReq.y);
  }
  deleteQueue.clear();

  for (const TileCreationRequest &createReq : createQueue) {
    createTile(createReq.layer, createReq.x, createReq.y, createReq.rect);
  }
  createQueue.clear();
}

void LevelManager::onSceneUnload() {
  for (auto &layer : layers) {
    for (auto &tile : layer.tiles) {
      TileFactory::destroy(tile.object);
    }

    layer.tiles.clear();
  }

  layers.clear();

  createQueue.clear();
  deleteQueue.clear();

  levelLayout.clear();

  backgroundColor = ColorPalette::DarkCyanBlue;
}

void LevelManager::setBackgroundColor(sf::Color newColor) {
  backgroundColor = newColor;
}

const std::string &LevelManager::getLoadedLevelPath() const {
  ensureLevelLoaded();
  return loadedLevelpath;
}

void LevelManager::setSecretLayerOppacity(float oppacity) {
  ensureLevelLoaded();
  auto it = std::ranges::find_if(
      layers, [](const LayerInfo &layer) { return layer.name == "secret"; });

  if (it != layers.end()) {
    LayerInfo &secretLayer = *it;
    for (auto &tile : secretLayer.tiles) {
      RenderableObject *obj = tile.object;
      if (obj) {
        obj->renderizer.setColor(
            sf::Color(255, 255, 255, static_cast<sf::Uint8>(oppacity)));
      }
    }
  }
}

void LevelManager::ensureLevelLoaded() const {
  if (!isLevelLoaded) {
    throw LevelManager::Error(
        "[LevelManager] Can't execute action. No level loaded");
  }
}

void LevelManager::initializeRenderContext(WindowManager &windowManager,
                                           WindowID window,
                                           GameCamera *camera) {
  renderContext.windowManager = &windowManager;
  renderContext.window = window;
  renderContext.camera = camera;
}

RenderizerParameters LevelManager::makeRenderParams(size_t layerNo) const {

  float layerValue = static_cast<float>(layerNo);
  if (layers[layerNo].name == "secret") {
    layerValue = -1;
  }

  return RenderizerParameters{.windowManager = *renderContext.windowManager,
                              .window = renderContext.window,
                              .texture = const_cast<sf::Texture *>(&tilesheet),
                              .camera = renderContext.camera,
                              .layer = layerValue,
                              .parallax = layers[layerNo].paralax};
}