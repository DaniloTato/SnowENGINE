#pragma once

#include <SFML/Graphics.hpp>
#include <limits>

struct OldRenderCommand {
  sf::IntRect rect;
  sf::Vector2f pos;
  sf::Color color;
  float overrideParalax = std::numeric_limits<float>::max();
};