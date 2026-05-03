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
  WindowManager::WindowID window;
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
  static LevelManager &getInstance();

  void loadLevel(WindowManager::WindowID window, GameCamera *camera,
                 const std::string &path);
  void loadLayer(WindowManager::WindowID window, GameCamera *camera,
                 size_t layerNo, const nlohmann::json &layerJSON, int tileSize);

  void createTile(WindowManager::WindowID window, GameCamera *camera,
                  int layerNo, int x, int y, sf::IntRect rect);

  void deleteTile(int layerNo, int x, int y);
  void saveLevel(const std::string &path);

  void deleteLayerObjects(int layerNo);
  void reloadAllLayers(WindowManager::WindowID window, GameCamera *camera);
  void reloadLayer(WindowManager::WindowID window, GameCamera *camera,
                   size_t layerNo);

  void queueCreateTile(WindowManager::WindowID window, GameCamera *camera,
                       int layer, int x, int y, const sf::IntRect &rect);
  void queueDeleteTile(int layer, int x, int y);
  void applyQueuedTileChanges();

  void setSecretLayerOppacity(float oppacity);

  const std::string &getLoadedLevelPath() const;
  const sf::Vector2f &getCameraPlayerRelation() const;

  void onSceneUnload() override;

  sf::Color &getBackgroundColor();
  void setBackgroundColor(sf::Color newColor);

  const LayerInfo getLayerInfo(int layerNo) const;

  const std::vector<std::vector<int>> &getLevelLayout() const;

  sf::Texture &getTilesheet();
  std::vector<LayerInfo> layers;
  int activeLayer;

  LevelManager(const LevelManager &) = delete;
  LevelManager &operator=(const LevelManager &) = delete;

  LevelManager(LevelManager &&) = delete;
  LevelManager &operator=(LevelManager &&) = delete;

private:
  LevelManager();

  sf::Texture tilesheet;
  sf::Vector2f cameraPlayerRelation;

  std::vector<std::vector<int>> levelLayout;
  std::vector<TileCreationRequest> createQueue;
  std::vector<TileDeletionRequest> deleteQueue;

  sf::Color backgroundColor;
  std::string loadedLevelpath;
  std::string tilesetPath;
};