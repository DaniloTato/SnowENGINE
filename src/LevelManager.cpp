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

using json = nlohmann::json;

LevelManager::LevelManager()
    : activeLayer(0),
      cameraPlayerRelation(Constants::STARTING_PLAYER_CAMERA_RELATION),
      backgroundColor(ColorPalette::Black) {}

LevelManager &LevelManager::getInstance() {
  static LevelManager instance;
  return instance;
}

sf::Color &LevelManager::getBackgroundColor() { return backgroundColor; }

void LevelManager::loadLevel(WindowManager::WindowID window, GameCamera *camera,
                             const std::string &path) {
  layers.clear();

  std::ifstream file(path);
  if (!file.is_open())
    throw std::runtime_error("Failed to open level file: " + path);

  loadedLevelpath = path;

  json data = json::parse(file);

  tilesetPath = data["tileset"].get<std::string>();

  tilesheet.loadFromFile(Helper::getPath(tilesetPath));
  int tileSize = data["tile_size"];

  auto &jsonLayers = data["layers"];
  layers.resize(jsonLayers.size());

  int width = 1000;
  int height = 1000;
  levelLayout.assign(height, std::vector<int>(width, 0));

  for (size_t i = 0; i < jsonLayers.size(); i++) {
    layers[i].name = jsonLayers[i]["name"].get<std::string>();
    layers[i].paralax = jsonLayers[i]["parallax"].get<float>();
    loadLayer(window, camera, i, jsonLayers[i], tileSize);

    // put secret layer above others
    if (layers[i].name == "secret") {
      for (auto &tile : layers[i].tiles) {
        tile.object->renderizer.setLayer(-1);
      }
    }
  }
}

void LevelManager::deleteLayerObjects(int layerNo) {
  LayerInfo layer = layers[layerNo];

  for (auto &tile : layer.tiles) {
    if (tile.object) {
      GameObject::destroy(tile.object);
      tile.object = nullptr;
    }
  }
}

void LevelManager::loadLayer(WindowManager::WindowID window, GameCamera *camera,
                             size_t layerNo, const json &layerJSON,
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

    RenderizerParameters params{.window = window,
                                .texture = &tilesheet,
                                .camera = camera,
                                .layer = static_cast<float>(layerNo),
                                .parallax = layers[layerNo].paralax};

    info.object = new RenderableObject(params);
    info.object->position = {float(info.x * tileSize),
                             float(info.y * tileSize)};
    info.object->renderizer.setRect(info.textureRect, 1);

    tileList.push_back(info);

    if (layerNo == 0 && unsignedInfoY < levelLayout.size() &&
        unsignedInfoX < levelLayout[0].size())
      levelLayout[unsignedInfoY][unsignedInfoX] = 1;
  }
}

void LevelManager::reloadAllLayers(WindowManager::WindowID window,
                                   GameCamera *camera) {
  for (size_t i = 0; i < layers.size(); ++i)
    reloadLayer(window, camera, i);
}

void LevelManager::reloadLayer(WindowManager::WindowID window,
                               GameCamera *camera, size_t layerNo) {
  auto &tiles = layers[layerNo].tiles;

  auto layerValue = static_cast<float>(layerNo);
  if (layers[layerNo].name == "secret") {
    layerValue = -1;
  }

  for (auto &t : tiles) {
    if (t.object) {
      GameObject::destroy(t.object);
      t.object = nullptr;
    }

    RenderizerParameters params{.window = window,
                                .texture = &tilesheet,
                                .camera = camera,
                                .layer = layerValue,
                                .parallax = layers[layerNo].paralax};

    t.object = new RenderableObject(params);
    t.object->position = {float(t.x * Constants::TILE_SIZE + 1),
                          float(t.y * Constants::TILE_SIZE + 1)};
    t.object->renderizer.setRect(t.textureRect, 1);
  }
}

void LevelManager::createTile(WindowManager::WindowID window,
                              GameCamera *camera, int layerNo, int x, int y,
                              sf::IntRect rect) {

  size_t unsignedX = x;
  size_t unsignedY = y;

  if (layerNo < 0 || layerNo >= (int)layers.size()) {
    std::cerr << "[createTile] ERROR: Invalid layerNo: " << layerNo << "\n";
    return;
  }

  if (layerNo == 0) {
    if (x < 0 || y < 0) {
      std::cerr << "[createTile] ERROR: Negative tile coordinates: (" << x
                << ", " << y << ")\n";
      return;
    }

    if ((unsignedY >= levelLayout.size() ||
         unsignedX >= levelLayout[y].size())) {
      std::cerr
          << "[createTile] ERROR: Tile coordinates out of levelLayout bounds: ("
          << x << ", " << y << ")\n";
      return;
    }
  }

  std::vector<TileInfo> &tiles = layers[layerNo].tiles;

  auto layerValue = static_cast<float>(layerNo);
  if (layers[layerNo].name == "secret") {
    layerValue = -1;
  }

  for (auto &t : tiles) {
    if (t.x == x && t.y == y) {
      if (t.object) {
        GameObject::destroy(t.object);
        t.object = nullptr;
      }

      if (layerNo == 0) {
        levelLayout[unsignedY][unsignedX] = 1;
      }

      t.textureRect = rect;

      RenderizerParameters params{.window = window,
                                  .texture = &tilesheet,
                                  .camera = camera,
                                  .layer = layerValue,
                                  .parallax = layers[layerNo].paralax};

      t.object = new RenderableObject(params);
      t.object->position = {float(x * Constants::TILE_SIZE + 1),
                            float(y * Constants::TILE_SIZE + 1)};
      t.object->renderizer.setRect(t.textureRect, 1);

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

  RenderizerParameters params{.window = window,
                              .texture = &tilesheet,
                              .camera = camera,
                              .layer = layerValue,
                              .parallax = layers[layerNo].paralax};

  info.object = new RenderableObject(params);
  info.object->position = {float(x * Constants::TILE_SIZE + 1),
                           float(y * Constants::TILE_SIZE + 1)};
  info.object->renderizer.assignCamera(camera);
  info.object->renderizer.setRect(info.textureRect, 1);

  tiles.push_back(info);
}

void LevelManager::deleteTile(int layerNo, int x, int y) {

  size_t unsignedX = x;
  size_t unsignedY = y;

  if (layerNo == 0 && (x < 0 || y < 0))
    return;

  std::vector<TileInfo> &tiles = layers[layerNo].tiles;

  std::erase_if(tiles, [&](TileInfo &t) {
    if (t.x == x && t.y == y) {
      if (t.object) {
        GameObject::destroy(t.object);
        t.object = nullptr;
      }
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
    std::cerr << "Failed to open " << path << " for writing!\n";
    return;
  }

  file << data.dump(4);
  file.close();
}

const std::vector<std::vector<int>> &LevelManager::getLevelLayout() const {
  return levelLayout;
}

sf::Texture &LevelManager::getTilesheet() { return tilesheet; }

const LayerInfo LevelManager::getLayerInfo(int layerNo) const {
  return layers[layerNo];
}

void LevelManager::queueCreateTile(WindowManager::WindowID window,
                                   GameCamera *camera, int layer, int x, int y,
                                   const sf::IntRect &rect) {
  createQueue.push_back({window, camera, layer, x, y, rect});
}

void LevelManager::queueDeleteTile(int layer, int x, int y) {
  deleteQueue.push_back({layer, x, y});
}

void LevelManager::applyQueuedTileChanges() {
  for (const auto &delReq : deleteQueue) {
    deleteTile(delReq.layer, delReq.x, delReq.y);
  }
  deleteQueue.clear();

  for (const auto &createReq : createQueue) {
    createTile(createReq.window, createReq.camera, createReq.layer, createReq.x,
               createReq.y, createReq.rect);
  }
  createQueue.clear();
}

const sf::Vector2f &LevelManager::getCameraPlayerRelation() const {
  return cameraPlayerRelation;
}

void LevelManager::onSceneUnload() {
  for (auto &layer : layers) {
    for (auto &tile : layer.tiles) {
      if (tile.object) {
        GameObject::destroy(tile.object);
        tile.object = nullptr;
      }
    }

    layer.tiles.clear();
  }

  layers.clear();

  createQueue.clear();
  deleteQueue.clear();

  levelLayout.clear();

  activeLayer = 0;
  cameraPlayerRelation = Constants::STARTING_PLAYER_CAMERA_RELATION;
  backgroundColor = ColorPalette::DarkCyanBlue;
}

void LevelManager::setBackgroundColor(sf::Color newColor) {
  backgroundColor = newColor;
}

const std::string &LevelManager::getLoadedLevelPath() const {
  return loadedLevelpath;
}

void LevelManager::setSecretLayerOppacity(float oppacity) {
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