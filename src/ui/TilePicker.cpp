#include "TilePicker.hpp"
#include "EnemyManager.hpp"
#include "Helpers.hpp"
#include "InputManager.hpp"
#include "UIButton.hpp"
#include <algorithm>

static auto createFont = [] {
  auto font = sf::Font();
  font.loadFromFile(Helper::getPath("assets/fonts/ARIAL.TTF"));
  return font;
};

TilePicker::TilePicker(sf::Texture &tileset, int tileSize)
    : font(createFont()), tileset(tileset), tileSize(tileSize) {
  selectedRect = {0, 0, tileSize, tileSize};
  selection.tileRect = selectedRect;
  parallaxSlider = std::make_unique<UISlider>(
      sf::Vector2f{10.f, float(tileset.getSize().y) + 100.f}, 500.f, -50.f,
      300.f, activeLayer >= 0 ? &(*layers)[activeLayer].paralax : nullptr);
}

void TilePicker::open() {
  if (opened)
    return;

  WindowManager &wm = WindowManager::getInstance();

  const unsigned int winW = tileset.getSize().x + 260;
  const unsigned int winH = tileset.getSize().y + 170;

  window = wm.create(WindowManager::Set::DEVUI, winW, winH, "TilePicker");
  wm.setFrameRate(window, 60);

  selectedRect = {0, 0, tileSize, tileSize};
  selection.tileRect = selectedRect;

  dragging = false;

  opened = true;
}

void TilePicker::update() {
  if (!opened)
    return;

  if (parallaxSlider && activeLayer >= 0 && layers) {
    parallaxSlider->bindTo(&(*layers)[activeLayer].paralax);
  }
}

void TilePicker::close() {
  if (!opened)
    return;

  // HORRIBLE BUG WAITING TO HAPPEN LOL
  LevelManager::getInstance().reloadAllLayers(
      WindowManager::getInstance().getMain(),
      GameState::getInstance().getMainCamera());

  WindowManager::getInstance().destroy(window);
  opened = false;
}

bool TilePicker::isOpen() const { return opened; }

PickerSelection TilePicker::getSelection() const { return selection; }

void TilePicker::handleEvent(WindowManager::WindowID id, const sf::Event &ev) {
  if (!opened || id != window)
    return;

  auto &im = InputManager::getInstance();

  if (ev.type == sf::Event::Closed) {
    close();
    return;
  }

  // =========================
  // MODE TABS (INPUT ONLY)
  // =========================
  {
    UIButton tilesBtn({10, 5}, {100, 26}, "Tiles", font);
    UIButton enemyBtn({120, 5}, {100, 26}, "Enemies", font);

    if (tilesBtn.isClicked(ev, window))
      selection.mode = PickerMode::Tiles;

    if (enemyBtn.isClicked(ev, window))
      selection.mode = PickerMode::Enemies;
  }

  // =========================
  // TILE DRAGGING
  // =========================
  if (selection.mode == PickerMode::Tiles) {

    if (ev.type == sf::Event::MouseButtonPressed &&
        ev.mouseButton.button == sf::Mouse::Left) {

      sf::Vector2i mp = im.getMousePosition(window);

      if (mp.x >= 0 && mp.x < (int)tileset.getSize().x && mp.y >= 40 &&
          mp.y < (int)tileset.getSize().y + 40) {

        dragging = true;
        dragStart = {mp.x, mp.y - 40};

        int tx = dragStart.x / tileSize;
        int ty = dragStart.y / tileSize;

        selectedRect = {tx * tileSize, ty * tileSize, tileSize, tileSize};
        selection.tileRect = selectedRect;
      }
    }

    if (ev.type == sf::Event::MouseMoved && dragging) {
      sf::Vector2i mp = im.getMousePosition(window);
      mp.y -= 40;

      int x1 = std::min(dragStart.x, mp.x) / tileSize * tileSize;
      int y1 = std::min(dragStart.y, mp.y) / tileSize * tileSize;
      int x2 = (std::max(dragStart.x, mp.x) / tileSize + 1) * tileSize;
      int y2 = (std::max(dragStart.y, mp.y) / tileSize + 1) * tileSize;

      selectedRect = {x1, y1, x2 - x1, y2 - y1};
      selection.tileRect = selectedRect;
    }

    if (ev.type == sf::Event::MouseButtonReleased &&
        ev.mouseButton.button == sf::Mouse::Left)
      dragging = false;
  }

  // =========================
  // ENEMY PICKER
  // =========================
  if (selection.mode == PickerMode::Enemies) {
    std::vector<std::string> enemies =
        EnemyManager::getInstance().getEnemyList();

    float x = 10.f;
    float y = 70.f;

    for (const auto &e : enemies) {
      UIButton btn({x, y}, {200, 24}, e, font);

      if (btn.isClicked(ev, window)) {
        selection.enemyId = e;
      }

      y += 32.f;
    }
  }

  // =========================
  // SLIDER
  // =========================
  if (parallaxSlider)
    parallaxSlider->handleEvent(ev, window);

  // =========================
  // LAYER UI INPUT
  // =========================
  if (layers) {
    float panelX = static_cast<float>(tileset.getSize().x) + 10.f;
    float y = 10.f;

    UIButton addBtn({panelX, y}, {240, 24}, "+ Add Layer", font);

    if (addBtn.isClicked(ev, window)) {
      LayerInfo n;
      n.name = "Layer " + std::to_string(layers->size());
      n.paralax = 1.0f;
      layers->push_back(n);

      if (activeLayer < 0)
        activeLayer = 0;

      return;
    }

    y += 40.f;

    for (int i = 0; i < (int)layers->size(); i++) {

      if (ev.type == sf::Event::MouseButtonReleased &&
          ev.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2i mp = im.getMousePosition(window);

        if ((float)mp.x >= panelX && (float)mp.x <= panelX + 240 &&
            (float)mp.y >= y && (float)mp.y <= y + 28) {
          activeLayer = i;
        }
      }

      UIButton up({panelX + 195, y}, {20, 12}, "", font);
      UIButton down({panelX + 195, y + 14}, {20, 12}, "", font);
      UIButton del({panelX + 220, y}, {20, 24}, "X", font);

      if (up.isClicked(ev, window) && i > 0) {
        std::swap((*layers)[i], (*layers)[i - 1]);
        activeLayer = i - 1;
        return;
      }

      if (down.isClicked(ev, window) && i < (int)layers->size() - 1) {
        std::swap((*layers)[i], (*layers)[i + 1]);
        activeLayer = i + 1;
        return;
      }

      if (del.isClicked(ev, window)) {
        LevelManager::getInstance().deleteLayerObjects(i);
        layers->erase(layers->begin() + i);

        if (layers->empty())
          activeLayer = -1;
        else if (activeLayer >= (int)layers->size())
          activeLayer = (int)layers->size() - 1;

        return;
      }

      y += 36;
    }
  }
}

void TilePicker::draw() {
  if (!opened)
    return;

  auto &wm = WindowManager::getInstance();
  wm.clearWindow(window, sf::Color(30, 30, 30));

  // MODE TABS (ONLY DRAW)
  {
    UIButton tilesBtn({10, 5}, {100, 26}, "Tiles", font);
    UIButton enemyBtn({120, 5}, {100, 26}, "Enemies", font);

    tilesBtn.draw(window);
    enemyBtn.draw(window);
  }

  if (selection.mode == PickerMode::Tiles)
    drawTileset(selectedRect);

  if (selection.mode == PickerMode::Enemies)
    drawEnemyPicker();

  if (layers) {
    drawLayerList(*layers, activeLayer);
  }

  if (activeLayer >= 0 && layers && parallaxSlider)
    drawParallaxUI((*layers)[activeLayer], *parallaxSlider);

  wm.checkRenderFlag(window);
}

void TilePicker::drawEnemyPicker() {
  auto enemies = EnemyManager::getInstance().getEnemyList();

  float x = 10.f;
  float y = 40.f;

  sf::Text title("Select Enemy:", font, 16);
  title.setPosition(x, y);
  WindowManager::getInstance().drawOnWindow(window, title);

  y += 30.f;

  for (const auto &e : enemies) {
    UIButton btn({x, y}, {200, 24}, e, font);
    btn.draw(window); // ONLY DRAW

    y += 32.f;
  }
}

void TilePicker::drawTileset(const sf::IntRect &selectedRect) {

  WindowManager &windowManager = WindowManager::getInstance();

  sf::Sprite spr(tileset);
  spr.setPosition(0.f, 40.f);

  windowManager.drawOnWindow(window, spr);

  int cols = static_cast<int>(tileset.getSize().x / tileSize);
  int rows = static_cast<int>(tileset.getSize().y / tileSize);

  std::vector<sf::Vertex> lines;

  for (int x = 0; x <= cols; x++) {
    lines.emplace_back(sf::Vector2f(static_cast<float>(x * tileSize), 40),
                       sf::Color(150, 150, 150));
    lines.emplace_back(sf::Vector2f(static_cast<float>(x * tileSize),
                                    static_cast<float>(rows * tileSize + 40)),
                       sf::Color(150, 150, 150));
  }
  for (int y = 0; y <= rows; y++) {
    lines.emplace_back(sf::Vector2f(0, static_cast<float>(y * tileSize + 40)),
                       sf::Color(150, 150, 150));
    lines.emplace_back(sf::Vector2f(static_cast<float>(cols * tileSize),
                                    static_cast<float>(y * tileSize + 40)),
                       sf::Color(150, 150, 150));
  }

  windowManager.drawOnWindow(window, lines.data(), lines.size(), sf::Lines);

  sf::RectangleShape sel(
      {(float)selectedRect.width, (float)selectedRect.height});
  sel.setPosition((float)selectedRect.left, (float)selectedRect.top + 40);
  sel.setFillColor(sf::Color::Transparent);
  sel.setOutlineColor(sf::Color::Yellow);
  sel.setOutlineThickness(2.f);
  windowManager.drawOnWindow(window, sel);
}

void TilePicker::drawLayerList(std::vector<LayerInfo> &layers,
                               int &activeLayer) {

  auto &wm = WindowManager::getInstance();

  float panelX = static_cast<float>(tileset.getSize().x) + 10.f;
  float y = 10.f;

  UIButton addBtn({panelX, y}, {240, 24}, "+ Add Layer", font);
  addBtn.draw(window);

  y += 40.f;

  for (int i = 0; i < (int)layers.size(); i++) {
    bool selected = (i == activeLayer);

    sf::RectangleShape entry({240, 28});
    entry.setPosition(panelX, y);
    entry.setFillColor(selected ? sf::Color(120, 120, 120)
                                : sf::Color(60, 60, 60));
    wm.drawOnWindow(window, entry);

    sf::Text t(layers[i].name, font, 14);
    t.setPosition(panelX + 10, y + 5);
    wm.drawOnWindow(window, t);

    UIButton up({panelX + 195, y}, {20, 12}, "", font);
    UIButton down({panelX + 195, y + 14}, {20, 12}, "", font);
    UIButton del({panelX + 220, y}, {20, 24}, "X", font);

    up.draw(window);
    down.draw(window);
    del.draw(window);

    y += 36;
  }
}

void TilePicker::drawParallaxUI(LayerInfo &layer, UISlider &slider) {
  float panelX = 10.f;
  float y = static_cast<float>(tileset.getSize().y) + 50.f;

  WindowManager &windowManager = WindowManager::getInstance();

  sf::Text label("Parallax:", font, 14);
  label.setPosition(panelX, y);
  windowManager.drawOnWindow(window, label);

  slider.draw(window);

  float rounded = float(int(layer.paralax * 100)) / 100.f;
  sf::Text v(std::to_string(rounded), font, 12);
  v.setPosition(10.f, y + 22);
  windowManager.drawOnWindow(window, v);
}

WindowManager::WindowID TilePicker::getWindow() const { return window; }

void TilePicker::setLayers(std::vector<LayerInfo> *l) {
  layers = l;
  if (layers && !layers->empty() && activeLayer < 0)
    activeLayer = 0;
}