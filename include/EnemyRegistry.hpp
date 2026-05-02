#pragma once

#include "EnemyManager.hpp"
#include <string>

class EnemyRegistry {
public:
  EnemyRegistry() = default;
  ~EnemyRegistry() = default;

  void loadEnemiesFromJSON(const std::string &path, EnemyManager &manager);

private:
  void registerEnemy(
      const std::string &id, const std::string &texturePath,
      const std::string &jsonPath, EnemyManager &enemyManager,
      std::function<TangibleObject *(const sf::Vector2f &)> blueprint);
};
