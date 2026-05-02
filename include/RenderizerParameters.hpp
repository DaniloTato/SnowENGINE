#pragma once

#include <SFML/Graphics.hpp>

class GameCamera;

struct RenderizerParameters {
  sf::RenderWindow *window = nullptr;
  sf::Texture *texture = nullptr;
  GameCamera *camera = nullptr;
  float layer = 0.f;
  float parallax = 1.f;
  bool registerAsRectShape = false;
};