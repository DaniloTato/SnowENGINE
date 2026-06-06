#pragma once
#include <SFML/Graphics.hpp>

struct TextGlyph {
  char c;
  sf::IntRect texRect;
  sf::Vector2f basePos;
  sf::Color color;

  enum Anim : std::uint8_t { None, Sin, Shake } anim = None;

  float animParam = 0.f;
  float phase = 0.f;
  int appearIndex = -1;
};