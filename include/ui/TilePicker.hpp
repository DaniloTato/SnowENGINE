#pragma once
#include "LevelManager.hpp"
#include "UISlider.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "IEventListener.hpp"

enum class PickerMode : std::uint8_t { Tiles, Enemies };

struct PickerSelection {
  PickerMode mode = PickerMode::Tiles;
  sf::IntRect tileRect;
  std::string enemyId;
  int activeLayer;
};

class TilePicker : public IEventListener {
public:
  TilePicker(sf::Texture &tileset, int tileSize, WindowManager &windowManager,
             LevelManager &levelManager);

  void open();
  void close(WindowManager &wm);

  void handleEvent(WindowID id, const sf::Event &ev) override;
  void update();
  void draw();

  void setLayers(std::vector<LayerInfo> *l);

  WindowID getWindow() const;

  [[nodiscard]] bool isOpen() const;
  [[nodiscard]] PickerSelection getSelection() const;

private:
  void drawTileset(const sf::IntRect &selectedRect);
  void drawLayerList(std::vector<LayerInfo> &layers, int &activeLayer);
  void drawParallaxUI(LayerInfo &layer, UISlider &slider);
  void drawEnemyPicker();

private:
  WindowID window;
  bool opened = false;

  sf::Font font;
  sf::Texture &tileset;
  WindowManager &windowManager;
  int tileSize;

  PickerSelection selection;

  bool dragging = false;
  sf::Vector2i dragStart;
  sf::IntRect selectedRect;

  int activeLayer = -1;
  std::vector<LayerInfo> *layers = nullptr;

  std::unique_ptr<UISlider> parallaxSlider;

  // Probably incorrect dependency.
  LevelManager &levelManager;
};