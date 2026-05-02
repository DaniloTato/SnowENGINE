#include "EnemyManager.hpp"
#include "GameObject.hpp"
#include "GameState.hpp"
#include "Helpers.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

EnemyManager &EnemyManager::getInstance() {
  static EnemyManager instance;
  return instance;
}

void EnemyManager::queueCreateEnemy(const std::string &templateName,
                                    const sf::Vector2f &position) {
  createQueue.push_back({templateName, position});
}

void EnemyManager::queueDeleteEnemy(TangibleObject *enemy) {
  deleteQueue.push_back(enemy);
}

void EnemyManager::destroyObject(TangibleObject *obj) {
  GameObject::destroy(obj);
}

std::vector<std::string> EnemyManager::getEnemyList() const {
  std::vector<std::string> result;
  result.reserve(templates.size());

  for (const auto &pair : templates) {
    result.push_back(pair.first);
  }

  return result;
}

TangibleObject *EnemyManager::isCollidingWithEnemy(TangibleObject &object) {
  for (TangibleObject *enemy : objects) {

    if (!enemy)
      continue;

    if (enemy->attackHitbox.has_value()) {
      if (BasicCollider::isCollidingRect(
              enemy->attackHitbox.value().collider.getCollisionRect(
                  enemy->position),
              object.collider.getCollisionRect(object.position))) {
        return enemy;
      }
    }

    /*Tight coupling problem
    Find a way to find if the enemy is dying or not. maybe add it as a member
    in Tangible Object

    if (script::DamageFunctions::isDying(*enemy))
      continue;

    */

    if (enemy->makesDamageTroughContact &&
        BasicCollider::objectsColliding(&object, enemy)) {
      return enemy;
    }
  }

  return nullptr;
}

void EnemyManager::checkSpawnTriggers(const sf::Vector2f &playerPos) {
  for (auto &def : spawnDefinitions) {
    if (!def.enabled)
      continue;

    if (Helper::distance(playerPos, def.spawnPoint) > def.activationRadius)
      continue;

    if (def.triggerType == EnemySpawnTriggerType::PROXIMITY) {
      if (!def.triggeredOnce) {
        queueCreateEnemy(def.templateName, def.spawnPoint);
        def.triggeredOnce = true;
      }
    }

    if (def.triggerType == EnemySpawnTriggerType::AREA_ENTER) {
      if (!def.triggeredOnce && def.triggerArea.contains(playerPos)) {
        queueCreateEnemy(def.templateName, def.spawnPoint);
        def.triggeredOnce = true;
      }
    }

    if (def.triggerType == EnemySpawnTriggerType::TIMER) {
      def.spawnTimer -= GameState::getInstance().dt();
      if (def.spawnTimer <= 0.f) {
        queueCreateEnemy(def.templateName, def.spawnPoint);
        def.spawnTimer = def.spawnInterval;
      }
    }
  }
}

void EnemyManager::loadSpawnDefinitionsFromJson(const std::string &path) {
  std::ifstream f(path);
  nlohmann::json j;
  f >> j;

  spawnDefinitions.clear();

  for (auto &s : j["spawns"]) {
    EnemySpawnDefinition def;
    def.id = s["id"];
    def.templateName = s["template"];

    std::string trigger = s["trigger"];

    if (trigger == "AREA_ENTER") {
      def.triggerType = EnemySpawnTriggerType::AREA_ENTER;
    } else if (trigger == "TIMER") {
      def.triggerType = EnemySpawnTriggerType::TIMER;
    } else if (trigger == "PROXIMITY") {
      def.triggerType = EnemySpawnTriggerType::PROXIMITY;
    }

    if (def.triggerType == EnemySpawnTriggerType::AREA_ENTER) {
      auto r = s["triggerArea"];
      def.triggerArea = {r[0], r[1], r[2], r[3]};
    }

    if (def.triggerType == EnemySpawnTriggerType::TIMER) {
      def.spawnInterval = s["interval"];
      def.spawnTimer = 0.f;
    }

    auto p = s["spawnPoint"];
    def.spawnPoint = {p[0], p[1]};
    def.activationRadius = s.value("activationRadius", 500.f);

    spawnDefinitions.push_back(def);
  }
}

void EnemyManager::saveSpawnDefinitionsToJson(const std::string &path) {
  nlohmann::json j;
  j["spawns"] = nlohmann::json::array();

  for (const auto &def : spawnDefinitions) {
    j["spawns"].push_back({{"id", def.id},
                           {"template", def.templateName},

                           {"triggerType", static_cast<int>(def.triggerType)},

                           {"triggerArea",
                            {def.triggerArea.left, def.triggerArea.top,
                             def.triggerArea.width, def.triggerArea.height}},

                           {"spawnPoint", {def.spawnPoint.x, def.spawnPoint.y}},

                           {"spawnInterval", def.spawnInterval},
                           {"maxAlive", def.maxAlive},
                           {"activationRadius", def.activationRadius},
                           {"triggeredOnce", def.triggeredOnce},
                           {"enabled", def.enabled}});
  }

  std::ofstream file(path);
  if (!file.is_open()) {
    std::cerr << "[EnemyManager] Failed to save spawn file: " << path << "\n";
    return;
  }

  file << j.dump(4);
}

void EnemyManager::addSpawnDefinition(const EnemySpawnDefinition &def) {
  spawnDefinitions.push_back(def);
}

void EnemyManager::removeSpawnDefinition(const std::string &id) {
  std::erase_if(spawnDefinitions,
                [&](const auto &def) { return def.id == id; });
}

void EnemyManager::removeSpawnDefinition(const sf::Vector2f &position,
                                         float tolerance) {
  const float tolSq = tolerance * tolerance;
  std::erase_if(spawnDefinitions, [&](const auto &def) {
    sf::Vector2f d = def.spawnPoint - position;
    return (d.x * d.x + d.y * d.y) <= tolSq;
  });
}

std::vector<EnemyManager::EnemySpawnDefinition> &
EnemyManager::getSpawnDefinitions() {
  return spawnDefinitions;
}