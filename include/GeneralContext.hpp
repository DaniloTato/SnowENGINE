#pragma once

#include "CameraManager.hpp"
#include "TangibleObject.hpp"
#include <SFML/System/Vector2.hpp>

struct GeneralContext {
  TangibleObject *player;
  CameraManager *cameraManager;
  CameraID mainCamera;
};

struct EmptyContext : public GeneralContext {
  EmptyContext()
      : GeneralContext{.player = nullptr,
                       .cameraManager = nullptr,
                       .mainCamera = CameraID{}} {}
};