#pragma once

#include "LevelManager.hpp"
#include "SceneManager.hpp"

class Engine {
public:
  WindowManager &getWindowManager() { return windowManager; }
  LevelManager &getLevelManager() { return levelManager; }
  SceneManager &getSceneManager() { return sceneManager; }
  RenderSystem &getRenderSystem() { return renderSystem; }

private:
  WindowManager windowManager;
  LevelManager levelManager;
  SceneManager sceneManager;
  RenderSystem renderSystem{windowManager};
};