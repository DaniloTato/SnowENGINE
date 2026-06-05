#pragma once

#include "Engine.hpp"
#include "TangibleObject.hpp"
#include <SFML/System/Vector2.hpp>

struct GeneralContext {
  TangibleObject *player;
  WindowID mainWindow;
  CameraComponent *mainCamera;
  Engine *engine;
  Scene *gameScene;
};

struct EmptyContext : public GeneralContext {
  EmptyContext()
      : GeneralContext{
            .player = nullptr,
            .mainWindow = WindowID(),
            .mainCamera = nullptr,
            .engine = nullptr,
            .gameScene = nullptr,
        } {}
};