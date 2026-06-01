#pragma once

#include "DialogueManager.hpp"
#include "LevelManager.hpp"
#include "SceneManager.hpp"

class Engine {
public:
  WindowManager &getWindowManager() { return windowManager; }
  LevelManager &getLevelManager() { return levelManager; }
  SceneManager &getSceneManager() { return sceneManager; }
  DialogueManager &getDialogueManager() { return dialogueManager; }

private:
  DialogueManager dialogueManager;
  WindowManager windowManager;
  LevelManager levelManager;
  SceneManager sceneManager;
};