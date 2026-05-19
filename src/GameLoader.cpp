#include "GameLoader.hpp"

#include "GameLoader.hpp"

#include "BlueprintRegistry.hpp"
#include "CollectableManager.hpp"
#include "EnemyRegistry.hpp"
#include "Helpers.hpp"
#include "SceneBuilderRegistry.hpp"
#include "SceneManager.hpp"
#include "SoundManager.hpp"
#include "TextureManager.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

void GameLoader::loadEnemies(const std::string &path) {
  EnemyRegistry registry;
  registry.loadEnemiesFromJSON(path, EnemyManager::getInstance());
}

void GameLoader::loadSounds(const std::string &path) {

  std::ifstream file(path);
  nlohmann::json data;
  file >> data;

  auto &soundManager = SoundManager::getInstance();

  for (auto &sound : data["sounds"]) {
    soundManager.load(sound["id"], Helper::getPath(sound["path"]));
  }
}

void GameLoader::loadCollectables(const std::string &path) {

  std::ifstream file(path);
  nlohmann::json data;
  file >> data;

  auto &collectableManager = CollectableManager::getInstance();

  for (auto &collectable : data["collectables"]) {

    std::string id = collectable["id"];
    std::string texture = collectable["texture"];
    std::string animation = collectable["animation"];
    std::string blueprint = collectable["blueprint"];

    collectableManager.textureCache.load(
        id, Helper::loadTexture(Helper::getPath(texture)));

    collectableManager.animationCache.load(
        id, Animator::getAsepriteJSONAnimations(Helper::getPath(animation)));

    auto blueprintFunc = BlueprintRegistry::get(blueprint);

    collectableManager.registerTemplate(id, blueprintFunc);
  }
}

void GameLoader::loadScenes(const std::string &path,
                            WindowManager &windowManager) {

  std::ifstream file(path);
  nlohmann::json data;
  file >> data;

  auto &sceneManager = SceneManager::getInstance();

  for (auto &[name, func] : SceneBuilderRegistry::getAllItems()) {
    sceneManager.registerScene(name, func);
  }

  sceneManager.loadScene(data["default"], windowManager);
}

void GameLoader::loadGameData(const std::string &configFolder,
                              WindowManager &windowManager) {
  loadEnemies(
      Helper::getPath(configFolder + "/enemieManagerDeclarations.json"));
  loadCollectables(
      Helper::getPath(configFolder + "/collectableManagerDeclarations.json"));
  loadSounds(Helper::getPath(configFolder + "/soundManagerDeclarations.json"));
  loadTextures(
      Helper::getPath(configFolder + "/textureManagerDeclarations.json"));
  loadScenes(Helper::getPath(configFolder + "/sceneManagerDeclarations.json"),
             windowManager);
}

void GameLoader::loadTextures(const std::string &path) {
  std::ifstream file(path);
  nlohmann::json data;
  file >> data;

  auto &textureManager = TextureManager::getInstance();

  for (auto &texture : data["textures"]) {
    std::string id = texture["id"];
    std::string path = texture["path"];
    textureManager.load(id, Helper::getPath(path));
  }
}