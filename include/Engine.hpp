#pragma once

#include "CameraManager.hpp"
#include "LevelManager.hpp"
#include "SceneManager.hpp"

class Engine {
public:
  CameraManager &getCameraManager() { return cameraManager; }
  WindowManager &getWindowManager() { return windowManager; }
  LevelManager &getLevelManager() { return levelManager; }
  SceneManager &getSceneManager() { return sceneManager; }

private:
  CameraManager cameraManager;
  WindowManager windowManager;
  LevelManager levelManager;
  SceneManager sceneManager;
};