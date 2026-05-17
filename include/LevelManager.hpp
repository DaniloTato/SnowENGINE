#pragma once
#include "GameCamera.hpp"
#include "RenderableObject.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SceneAware.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct TileCreationRequest {
  WindowID window;
  GameCamera *camera;
  int layer;
  int x;
  int y;
  sf::IntRect rect;
};

struct TileDeletionRequest {
  int layer;
  int x;
  int y;
};

struct TileInfo {
  int x, y;
  sf::IntRect textureRect;
  RenderableObject *object = nullptr;
};

struct LayerInfo {
  float paralax = 1.f;
  std::string name = "default";
  std::vector<TileInfo> tiles;
};

class LevelManager : public SceneAware {
public:
  // Later Expand to error pipeline for the whole engine.
  struct Error {
    Error(std::string &&message) : message(message) {}
    std::string message;
  };

  static LevelManager &getInstance();

  void loadLevel(WindowID window, GameCamera *camera, const std::string &path);
  void saveLevel(const std::string &path);

  void queueCreateTile(WindowID window, GameCamera *camera, int layer, int x,
                       int y, const sf::IntRect &rect);
  void queueDeleteTile(int layer, int x, int y);
  void applyQueuedTileChanges();

  void reloadAllLayers(WindowID window, GameCamera *camera);
  void reloadLayer(WindowID window, GameCamera *camera, size_t layerNo);

  void setSecretLayerOppacity(float oppacity);

  const std::string &getLoadedLevelPath() const;

  void onSceneUnload() override;

  sf::Color &getBackgroundColor();
  void setBackgroundColor(sf::Color newColor);

  const LayerInfo getLayerInfo(int layerNo) const;
  void deleteLayerObjects(int layerNo);

  const std::vector<std::vector<int>> &getLevelLayout() const;

  sf::Texture &getTilesheet();

  // MUST Refactor later to turn them into private members
  std::vector<LayerInfo> layers;
  int activeLayer;

  LevelManager(const LevelManager &) = delete;
  LevelManager &operator=(const LevelManager &) = delete;

  LevelManager(LevelManager &&) = delete;
  LevelManager &operator=(LevelManager &&) = delete;

private:
  void createTile(WindowID window, GameCamera *camera, int layerNo, int x,
                  int y, sf::IntRect rect);
  void deleteTile(int layerNo, int x, int y);

  void loadLayer(WindowID window, GameCamera *camera, size_t layerNo,
                 const nlohmann::json &layerJSON, int tileSize);

  void ensureLevelLoaded() const;

  LevelManager();

  bool isLevelLoaded;

  sf::Texture tilesheet;

  std::vector<std::vector<int>> levelLayout;
  std::vector<TileCreationRequest> createQueue;
  std::vector<TileDeletionRequest> deleteQueue;

  sf::Color backgroundColor;
  std::string loadedLevelpath;
  std::string tilesetPath;

  // SHOULD MAKE WINDOW AND CAMERA MEMBERS OF THE MANAGER SOMEHOW
};