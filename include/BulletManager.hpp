#pragma once
#include "Animation.hpp"
#include "BasicCollider.hpp"
#include "Bullet.hpp"
#include "QueuedManager.hpp"

struct BulletCreationRequest {
  RenderizerParameters params;
  Bullet::Type type;
  sf::Vector2f pos;
  sf::Vector2f speed;
  sf::Vector2f accel;
  float damageRadius;
  float range;
  bool shotByPlayer;
  LevelManager::LevelLayout2D &layout;
};

class BulletManager : public QueuedManager<Bullet, BulletCreationRequest> {
public:
  inline static const bool I_AM_PLAYER = true;
  inline static const bool I_AM_NOT_PLAYER = false;

  static BulletManager &getInstance();

  void queueSpawn(RenderizerParameters &params, Bullet::Type type,
                  const sf::Vector2f &pos, const sf::Vector2f &speed,
                  const sf::Vector2f &accel, float damageRadius, float range,
                  bool shotByPlayer, LevelManager::LevelLayout2D &layout);

  void queueDeletion(Bullet *bullet);

  void update();

  Bullet *isCollidingWithBullet(TangibleObject &object, bool amIPlayer);

private:
  BulletManager();

  Bullet *createFromRequest(const BulletCreationRequest &req) override;
  void destroyObject(Bullet *bullet) override;

  Animations cachedAnimations;
};