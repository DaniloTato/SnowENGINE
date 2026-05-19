#pragma once

#include "CameraManager.hpp"

class Engine {
public:
  CameraManager &getCameraManager() { return cameraManager; }
  WindowManager &getWindowManager() { return windowManager; }

private:
  CameraManager cameraManager;
  WindowManager windowManager;
};