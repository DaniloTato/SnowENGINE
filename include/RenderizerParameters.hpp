#pragma once

#include "CameraID.hpp"
#include "WindowID.hpp"
#include <SFML/Graphics.hpp>

class GameCamera;
class Engine;

struct RenderizerParameters {
  Engine &engine;
  WindowID window;
  sf::Texture *texture = nullptr;
  CameraID camera;
  float layer = 0.f;
  float parallax = 1.f;
  bool registerAsRectShape = false;
};