#pragma once

#include "CameraManager.hpp"

class Engine {
public:
  CameraManager &getCameraManager() { return cameraManager; }

private:
  CameraManager cameraManager;
};