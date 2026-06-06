#include "BasicCollider.hpp"
#include "Constants.hpp"
#include "LevelManager.hpp"
#include "SFML/System/Vector2.hpp"
#include "TangibleObject.hpp"
#include <cmath>

bool BasicCollider::objectsColliding(TangibleObject *object1,
                                     TangibleObject *object2) {
  // maybe add dependecy collider-gameObject?
  return isCollidingRect(object1->collider.getCollisionRect(object1->position),
                         object2->collider.getCollisionRect(object2->position));
}

bool BasicCollider::isCollidingRect(const sf::FloatRect &a,
                                    const sf::FloatRect &b) {
  return a.intersects(b);
}

void BasicCollider::setSize(const sf::Vector2f &newSize) { size = newSize; }

void BasicCollider::setOffset(const sf::Vector2f &newOffset) {
  offset = newOffset;
}

sf::FloatRect
BasicCollider::getCollisionRect(const sf::Vector2f &objectPos) const {
  return {objectPos.x + offset.x, objectPos.y + offset.y, size.x, size.y};
}

void BasicCollider::computeCollisionGrid(const sf::Vector2f &objectPos) {
  tileCoords.clear();
  const int TILE = Constants::TILE_SIZE;

  sf::FloatRect box = getCollisionRect(objectPos);

  int x0 = static_cast<int>(std::floor(box.left / TILE));
  int y0 = static_cast<int>(std::floor(box.top / TILE));
  int x1 = static_cast<int>(std::floor((box.left + box.width) / TILE));
  int y1 = static_cast<int>(std::floor((box.top + box.height) / TILE));

  for (int y = y0; y <= y1; ++y)
    for (int x = x0; x <= x1; ++x)
      tileCoords.emplace_back(x, y);
}

bool BasicCollider::horizontalLevelCollision(
    sf::Vector2f &objectPos, LevelManager::LevelLayout2D &level) {
  const int TILE = Constants::TILE_SIZE;

  computeCollisionGrid(objectPos);

  sf::FloatRect rect = getCollisionRect(objectPos);
  bool collided = false;

  for (auto &t : tileCoords) {
    size_t tx = t.x, ty = t.y;

    if (ty >= level.size())
      continue;
    if (tx >= level[ty].size())
      continue;

    if (level[ty][tx] != 0) {
      const auto TILE_T = static_cast<size_t>(TILE);
      sf::FloatRect tileBox(static_cast<float>(tx * TILE_T),
                            static_cast<float>(ty * TILE),
                            static_cast<float>(TILE), static_cast<float>(TILE));

      if (rect.intersects(tileBox)) {
        collided = true;

        bool movingRight = objectPos.x > lastFramePos.x;

        if (movingRight) {
          objectPos.x = tileBox.left - size.x - offset.x;
        } else {
          objectPos.x = tileBox.left + TILE - offset.x;
        }

        rect = getCollisionRect(objectPos);
      }
    }
  }

  lastFramePos.x = objectPos.x;
  return collided;
}

bool BasicCollider::verticalLevelCollision(sf::Vector2f &objectPos,
                                           LevelManager::LevelLayout2D &level) {

  const int TILE = Constants::TILE_SIZE;

  computeCollisionGrid(objectPos);

  sf::FloatRect rect = getCollisionRect(objectPos);
  bool collided = false;

  for (auto &t : tileCoords) {
    size_t tx = t.x, ty = t.y;

    if (ty >= level.size())
      continue;
    if (tx >= level[ty].size())
      continue;

    if (level[ty][tx] != 0) {

      sf::FloatRect tileBox(static_cast<float>(tx * TILE),
                            static_cast<float>(ty * TILE),
                            static_cast<float>(TILE), static_cast<float>(TILE));

      if (rect.intersects(tileBox)) {

        collided = true;
        bool movingDown = objectPos.y > lastFramePos.y;

        if (movingDown) {
          objectPos.y = tileBox.top - size.y - offset.y;
        } else {
          objectPos.y = tileBox.top + TILE - offset.y;
        }

        rect = getCollisionRect(objectPos);
      }
    }
  }

  lastFramePos.y = objectPos.y;
  return collided;
}