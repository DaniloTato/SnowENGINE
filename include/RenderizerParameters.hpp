#pragma once

#include "WindowID.hpp"
#include <SFML/Graphics.hpp>

class GameCamera;

struct RenderizerParameters {
  WindowID window;
  sf::Texture *texture = nullptr;
  GameCamera *camera = nullptr;
  float layer = 0.f;
  float parallax = 1.f;
  bool registerAsRectShape = false;
};