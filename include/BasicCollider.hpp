#pragma once

#include "LevelManager.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

class TangibleObject;
class RenderableObject;

class BasicCollider {

public:
  BasicCollider() = default;

  void setSize(const sf::Vector2f &newSize);
  void setOffset(const sf::Vector2f &newOffset);

  [[nodiscard]] const sf::Vector2f &getSize() const { return size; }
  [[nodiscard]] const sf::Vector2f &getOffset() const { return offset; }

  [[nodiscard]] sf::FloatRect
  getCollisionRect(const sf::Vector2f &objectPos) const;
  bool horizontalLevelCollision(sf::Vector2f &objectPos,
                                LevelManager::LevelLayout2D &level);
  bool verticalLevelCollision(sf::Vector2f &objectPos,
                              LevelManager::LevelLayout2D &level);
  void computeCollisionGrid(const sf::Vector2f &objectPos);

  static bool objectsColliding(TangibleObject *object1,
                               TangibleObject *object2);
  static bool isCollidingRect(const sf::FloatRect &a, const sf::FloatRect &b);
  static bool
  tangibleAndRenderableCollision(TangibleObject *tangibleObject,
                                 RenderableObject *renderableObject);

private:
  sf::Vector2f size = {16.f, 16.f};
  sf::Vector2f offset = {0.f, 0.f};

  sf::Vector2f lastFramePos{0.f, 0.f};

  std::vector<sf::Vector2i> tileCoords;
};