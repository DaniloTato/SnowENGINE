#pragma once

#include <SFML/Graphics.hpp>

class GameCamera;

struct RenderizerParameters {
  sf::RenderWindow &window;
  sf::Texture &texture;
  GameCamera *camera;
  float layer = 0.f;
  float parallax = 1.f;
  bool registerAsRectShape = false;
};