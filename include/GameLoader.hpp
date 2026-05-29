#pragma once

#include "Engine.hpp"
#include <string>

class GameLoader {
public:
  void loadEnemies(const std::string &path);
  void loadCollectables(const std::string &path);
  void loadSounds(const std::string &path);
  void loadScenes(const std::string &path, Engine &engine);
  void loadTextures(const std::string &path);

  void loadGameData(const std::string &configFolder, Engine &engine);
};