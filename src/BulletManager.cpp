#include "BulletManager.hpp"
#include "BasicCollider.hpp"
#include "Helpers.hpp"
#include "SFML/System/Vector2.hpp"

BulletManager::BulletManager()
    : cachedAnimations(Animator::getAsepriteJSONAnimations(
          Helper::getPath("assets/json/bullet.json"))) {}

/* HARDCODED BULLET ANIMATION */

BulletManager &BulletManager::getInstance() {
  static BulletManager instance;
  return instance;
}

void BulletManager::queueSpawn(RenderizerParameters &params, Bullet::Type type,
                               const sf::Vector2f &pos,
                               const sf::Vector2f &speed,
                               const sf::Vector2f &accel, float damageRadius,
                               float range, bool shotByPlayer,
                               LevelManager::LevelLayout2D &layout) {
  createQueue.push_back({params, type, pos, speed, accel, damageRadius, range,
                         shotByPlayer, layout});
}

void BulletManager::queueDeletion(Bullet *bullet) {
  deleteQueue.push_back(bullet);
}

Bullet *BulletManager::createFromRequest(const BulletCreationRequest &req) {
  auto *bullet = new Bullet(req.params, cachedAnimations, req.type, req.pos,
                            req.speed, req.accel, req.damageRadius, req.range,
                            req.shotByPlayer, req.layout);

  bullet->animator.play("fly");
  return bullet;
}

void BulletManager::destroyObject(Bullet *bullet) { bullet->destroyLater(); }

void BulletManager::update() {
  for (Bullet *bullet : objects) {
    if (bullet->isDead()) {
      queueDeletion(bullet);
    }
  }
}

Bullet *BulletManager::isCollidingWithBullet(TangibleObject &object,
                                             bool amIPlayer) {

  for (Bullet *bullet : objects) {

    if (!bullet->isDying() &&
        BasicCollider::objectsColliding(&object, bullet) &&
        ((amIPlayer && !bullet->isShotByPlayer()) ||
         (!amIPlayer && bullet->isShotByPlayer()))) {
      return bullet;
    }
  }
  return nullptr;
}