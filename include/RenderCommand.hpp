#pragma once

#include "WindowID.hpp"
#include <SFML/Graphics.hpp>

struct RenderCommand {
  const sf::Texture *texture = nullptr;
  sf::Vector2f position;
  sf::IntRect textureRect;
  sf::Color color = sf::Color::White;
  sf::Vector2f scale = {1.f, 1.f};
  float layer = 0.f;
  float paralax = 1.f;
  WindowID window;
  bool isRectangle = false;
};