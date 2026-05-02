#include "TilePicker.hpp"
#include "EnemyManager.hpp"
#include "Helpers.hpp"
#include "UIButton.hpp"
#include <algorithm>

TilePicker::TilePicker(sf::Texture &tileset, int tileSize)
    : tileset(tileset), tileSize(tileSize) {
  selectedRect = {0, 0, tileSize, tileSize};
  selection.tileRect = selectedRect;
}

PickerSelection TilePicker::open(std::vector<LayerInfo> &layers,
                                 int &activeLayer) {
  unsigned int winW = tileset.getSize().x + 260;
  unsigned int winH = tileset.getSize().y + 170;

  sf::RenderWindow window(sf::VideoMode(winW, winH), "Tile / Enemy Picker");
  window.setFramerateLimit(60);

  sf::Font font;
  font.loadFromFile((Helper::getPath("assets/fonts/ARIAL.TTF")));

  if (layers.empty())
    activeLayer = -1;
  else if (activeLayer < 0 || activeLayer >= (int)layers.size())
    activeLayer = 0;

  float dummyMin = 0.1f;
  float dummyMax = 50.0f;

  UISlider parallaxSlider(
      {10.f, float(tileset.getSize().y) + 100.f}, 300.f, dummyMin, dummyMax,
      activeLayer >= 0 ? &layers[activeLayer].paralax : nullptr);

  while (window.isOpen()) {
    sf::Event ev;
    while (window.pollEvent(ev)) {
      if (ev.type == sf::Event::Closed)
        window.close();

      drawModeTabs(window, font, ev);

      // Tiles Mode
      if (selection.mode == PickerMode::Tiles) {
        if (ev.type == sf::Event::MouseButtonPressed &&
            ev.mouseButton.button == sf::Mouse::Left) {
          sf::Vector2i mp = sf::Mouse::getPosition(window);
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
          sf::Vector2i mp = sf::Mouse::getPosition(window);
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

      if (selection.mode == PickerMode::Enemies)
        drawEnemyPicker(window, font, ev);

      drawLayerList(window, layers, activeLayer, font, ev);

      if (activeLayer >= 0 && activeLayer < (int)layers.size())
        parallaxSlider.bindTo(&layers[activeLayer].paralax);

      if (activeLayer >= 0)
        parallaxSlider.handleEvent(ev, window);
    }

    window.clear(sf::Color(30, 30, 30));

    drawModeTabs(window, font, sf::Event{});

    if (selection.mode == PickerMode::Tiles)
      drawTileset(window, selectedRect);

    if (selection.mode == PickerMode::Enemies)
      drawEnemyPicker(window, font, sf::Event{});

    drawLayerList(window, layers, activeLayer, font, sf::Event{});

    if (activeLayer >= 0)
      drawParallaxUI(window, layers[activeLayer], font, parallaxSlider);

    window.display();
  }

  return selection;
}

void TilePicker::drawModeTabs(sf::RenderWindow &window, sf::Font &font,
                              const sf::Event &ev) {
  UIButton tilesBtn({10, 5}, {100, 26}, "Tiles", font);
  UIButton enemyBtn({120, 5}, {100, 26}, "Enemies", font);

  tilesBtn.draw(window);
  enemyBtn.draw(window);

  if (tilesBtn.isClicked(ev, window))
    selection.mode = PickerMode::Tiles;

  if (enemyBtn.isClicked(ev, window))
    selection.mode = PickerMode::Enemies;
}

void TilePicker::drawEnemyPicker(sf::RenderWindow &window, sf::Font &font,
                                 const sf::Event &ev) {
  std::vector<std::string> enemies = EnemyManager::getInstance().getEnemyList();

  float x = 10.f;
  float y = 40.f;

  sf::Text title("Select Enemy:", font, 16);
  title.setPosition(x, y);
  window.draw(title);

  y += 30.f;

  for (const auto &e : enemies) {
    UIButton btn({x, y}, {200, 24}, e, font);
    btn.draw(window);

    if (btn.isClicked(ev, window)) {
      selection.enemyId = e;
      selection.mode = PickerMode::Enemies;
    }

    y += 32.f;
  }
}

void TilePicker::drawTileset(sf::RenderWindow &window,
                             const sf::IntRect &selectedRect) {
  sf::Sprite spr(tileset);
  spr.setPosition(0.f, 40.f);
  window.draw(spr);

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

  window.draw(lines.data(), lines.size(), sf::Lines);

  sf::RectangleShape sel(
      {(float)selectedRect.width, (float)selectedRect.height});
  sel.setPosition((float)selectedRect.left, (float)selectedRect.top + 40);
  sel.setFillColor(sf::Color::Transparent);
  sel.setOutlineColor(sf::Color::Yellow);
  sel.setOutlineThickness(2.f);
  window.draw(sel);
}

void TilePicker::drawLayerList(sf::RenderWindow &window,
                               std::vector<LayerInfo> &layers, int &activeLayer,
                               sf::Font &font, const sf::Event &ev) {
  float panelX = static_cast<float>(tileset.getSize().x) + 10.f;
  float y = 10.f;

  UIButton addBtn({panelX, y}, {240, 24}, "+ Add Layer", font);
  addBtn.draw(window);

  if (addBtn.isClicked(ev, window)) {
    LayerInfo n;
    n.name = "Layer " + std::to_string(layers.size());
    n.paralax = 1.0f;
    layers.push_back(n);
    if (activeLayer < 0)
      activeLayer = 0;
    return;
  }

  y += 40.f;

  for (int i = 0; i < (int)layers.size(); i++) {
    bool selected = (i == activeLayer);

    sf::RectangleShape entry({240, 28});
    entry.setPosition(panelX, y);
    entry.setFillColor(selected ? sf::Color(120, 120, 120)
                                : sf::Color(60, 60, 60));
    window.draw(entry);

    sf::Text t(layers[i].name, font, 14);
    t.setPosition(panelX + 10, y + 5);
    window.draw(t);

    if (ev.type == sf::Event::MouseButtonReleased &&
        ev.mouseButton.button == sf::Mouse::Left) {
      sf::Vector2i mp = sf::Mouse::getPosition(window);
      if (static_cast<float>(mp.x) >= panelX &&
          static_cast<float>(mp.x) <= panelX + 240 &&
          static_cast<float>(mp.y) >= y && static_cast<float>(mp.y) <= y + 28)
        activeLayer = i;
    }

    UIButton up({panelX + 195, y}, {20, 12}, "", font, sf::Color::Yellow);
    up.draw(window);
    if (up.isClicked(ev, window) && i > 0) {
      std::swap(layers[i], layers[i - 1]);
      activeLayer = i - 1;
      return;
    }

    UIButton down({panelX + 195, y + 14}, {20, 12}, "", font,
                  sf::Color::Yellow);
    down.draw(window);
    if (down.isClicked(ev, window) && i < (int)layers.size() - 1) {
      std::swap(layers[i], layers[i + 1]);
      activeLayer = i + 1;
      return;
    }

    UIButton del({panelX + 220, y}, {20, 24}, "X", font);
    del.draw(window);
    if (del.isClicked(ev, window)) {
      LevelManager::getInstance().deleteLayerObjects(i);
      layers.erase(layers.begin() + i);
      if (layers.empty())
        activeLayer = -1;
      else if (activeLayer >= (int)layers.size())
        activeLayer = static_cast<int>(layers.size()) - 1;
      return;
    }

    y += 36;
  }
}

void TilePicker::drawParallaxUI(sf::RenderWindow &window, LayerInfo &layer,
                                sf::Font &font, UISlider &slider) {
  float panelX = 10.f;
  float y = static_cast<float>(tileset.getSize().y) + 50.f;

  sf::Text label("Parallax:", font, 14);
  label.setPosition(panelX, y);
  window.draw(label);

  slider.draw(window);

  float rounded = float(int(layer.paralax * 100)) / 100.f;
  sf::Text v(std::to_string(rounded), font, 12);
  v.setPosition(10.f, y + 22);
  window.draw(v);
}