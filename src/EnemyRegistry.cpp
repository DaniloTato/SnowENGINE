#include "EnemyRegistry.hpp"
#include "BlueprintRegistry.hpp"
#include "Helpers.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

void EnemyRegistry::loadEnemiesFromJSON(const std::string &path,
                                        EnemyManager &manager) {
  std::ifstream file(path);
  nlohmann::json data;
  file >> data;

  for (auto &enemy : data["enemies"]) {
    std::string id = enemy["id"];
    std::string texture = enemy["texture"];
    std::string anim = enemy["animation"];
    std::string blueprintName = enemy["blueprint"];

    auto blueprint = BlueprintRegistry::get(blueprintName);

    registerEnemy(id, texture, anim, manager, blueprint);
  }
}

void EnemyRegistry::registerEnemy(
    const std::string &id, const std::string &texturePath,
    const std::string &jsonPath, EnemyManager &enemyManager,
    std::function<TangibleObject *(const sf::Vector2f &)> blueprint) {
  enemyManager.textureCache.load(
      id, Helper::loadTexture(Helper::getPath(texturePath)));

  enemyManager.animationCache.load(
      id, Animator::getAsepriteJSONAnimations(Helper::getPath(jsonPath)));

  enemyManager.registerTemplate(id, std::move(blueprint));
}