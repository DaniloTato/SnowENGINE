#pragma once

#include <SFML/Graphics.hpp>

class Engine;

struct RenderizerParameters {
  Engine &engine;
  sf::Texture *texture = nullptr;
  float layer = 0.f;
  float parallax = 1.f;
  bool registerAsRectShape = false;
};