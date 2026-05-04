#pragma once
#include "LevelManager.hpp"
#include "UISlider.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class PickerMode : std::uint8_t { Tiles, Enemies };

struct PickerSelection {
  PickerMode mode = PickerMode::Tiles;
  sf::IntRect tileRect;
  std::string enemyId;
};

class TilePicker {
public:
  TilePicker(sf::Texture &tileset, int tileSize);

  void open();
  void close();

  void handleEvent(WindowManager::WindowID id, const sf::Event &ev);
  void update();
  void draw();

  void setLayers(std::vector<LayerInfo> *l);

  WindowManager::WindowID getWindow() const;

  [[nodiscard]] bool isOpen() const;
  [[nodiscard]] PickerSelection getSelection() const;

private:
  void drawTileset(const sf::IntRect &selectedRect);
  void drawLayerList(std::vector<LayerInfo> &layers, int &activeLayer);
  void drawParallaxUI(LayerInfo &layer, UISlider &slider);
  void drawEnemyPicker();

private:
  WindowManager::WindowID window;
  bool opened = false;

  sf::Font font;
  sf::Texture &tileset;
  int tileSize;

  PickerSelection selection;

  bool dragging = false;
  sf::Vector2i dragStart;
  sf::IntRect selectedRect;

  int activeLayer = -1;
  std::vector<LayerInfo> *layers = nullptr;

  std::unique_ptr<UISlider> parallaxSlider;
};