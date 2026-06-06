#pragma once
#include "FontAtlas.hpp"
#include "TextProperties.hpp"
#include <SFML/Graphics.hpp>
#include <string>

class TextComponent {
public:
  std::string markup;
  FontAtlas fontAtlas;
  TextAlign alignment = TextAlign::Left;
  TextEffect effect = TextEffect::None;
  float boundary = 100000.f;
  float typeDelay = 0.04f;
  float typeTimer = 0.f;
  size_t visibleChars = 0;
  size_t revealableCount = 0;
  bool dirty = true;

  std::vector<std::vector<size_t>> lines;

  sf::Vector2f origin = {0.f, 0.f};
  float lineSpacing = 2.f;

  void setMarkup(std::string markupText) {
    markup = std::move(markupText);
    dirty = true;
  }
};