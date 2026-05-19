#pragma once

#include "WindowManager.hpp"
#include <string>

class GameLoader {
public:
  void loadEnemies(const std::string &path);
  void loadCollectables(const std::string &path);
  void loadSounds(const std::string &path);
  void loadScenes(const std::string &path, WindowManager &windowManager);
  void loadTextures(const std::string &path);

  void loadGameData(const std::string &configFolder,
                    WindowManager &windowManager);
};