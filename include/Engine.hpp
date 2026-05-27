#pragma once

#include "CameraManager.hpp"
#include "DialogueManager.hpp"
#include "LevelManager.hpp"
#include "SceneManager.hpp"

class Engine {
public:
  CameraManager &getCameraManager() { return cameraManager; }
  WindowManager &getWindowManager() { return windowManager; }
  LevelManager &getLevelManager() { return levelManager; }
  SceneManager &getSceneManager() { return sceneManager; }
  DialogueManager &getDialogueManager() { return dialogueManager; }

private:
  DialogueManager dialogueManager;
  CameraManager cameraManager;
  WindowManager windowManager;
  LevelManager levelManager;
  SceneManager sceneManager;
};