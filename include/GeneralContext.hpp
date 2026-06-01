#pragma once

#include "Engine.hpp"
#include "TangibleObject.hpp"
#include <SFML/System/Vector2.hpp>

struct GeneralContext {
  TangibleObject *player;
  GameCamera *mainCamera;
  Engine *engine;
};

struct EmptyContext : public GeneralContext {
  EmptyContext()
      : GeneralContext{
            .player = nullptr, .mainCamera = nullptr, .engine = nullptr} {}
};