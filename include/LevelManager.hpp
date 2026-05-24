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

  struct LevelRenderContext {
    WindowManager *windowManager = nullptr;
    WindowID window;
    GameCamera *camera = nullptr;
  };

  class TileFactory {
  public:
    static RenderableObject *create(const TileInfo &tile,
                                    const RenderizerParameters &params,
                                    int tileSize);
    static void destroy(RenderableObject *&obj);
  };

  using LevelLayout2D = const std::vector<std::vector<int>>;

  LevelManager();
  LevelManager(const WindowManager &) = delete;
  LevelManager &operator=(const WindowManager &) = delete;
  LevelManager(WindowManager &&) = delete;
  LevelManager &operator=(WindowManager &&) = delete;

  void initializeRenderContext(WindowManager &windowManager, WindowID window,
                               GameCamera *camera);

  void loadLevel(const std::string &path);
  void saveLevel(const std::string &path);

  void queueCreateTile(int layer, int x, int y, const sf::IntRect &rect);
  void queueDeleteTile(int layer, int x, int y);
  void applyQueuedTileChanges();

  void reloadAllLayers();
  void reloadLayer(size_t layerNo);

  void setSecretLayerOppacity(float oppacity);

  const std::string &getLoadedLevelPath() const;

  void onSceneUnload() override;

  sf::Color &getBackgroundColor();
  void setBackgroundColor(sf::Color newColor);

  const LayerInfo getLayerInfo(int layerNo) const;
  void deleteLayerObjects(int layerNo);

  LevelLayout2D &getLevelLayout() const;

  sf::Texture &getTilesheet();

  // MUST Refactor later to turn them into private members
  std::vector<LayerInfo> layers;

private:
  void createTile(int layerNo, int x, int y, sf::IntRect rect);
  void deleteTile(int layerNo, int x, int y);

  void loadLayer(size_t layerNo, const nlohmann::json &layerJSON, int tileSize);

  void ensureLevelLoaded() const;

  RenderizerParameters makeRenderParams(size_t layerNo) const;

  bool isLevelLoaded;

  sf::Texture tilesheet;

  std::vector<std::vector<int>> levelLayout;
  std::vector<TileCreationRequest> createQueue;
  std::vector<TileDeletionRequest> deleteQueue;

  sf::Color backgroundColor;
  std::string loadedLevelpath;
  std::string tilesetPath;

  LevelRenderContext renderContext;
};