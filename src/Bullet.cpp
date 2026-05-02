#include "Bullet.hpp"
#include "ColorPalette.hpp"
#include "Constants.hpp"
#include "PhysicsComponent.hpp"
#include <cmath>

Bullet::Bullet(RenderizerParameters params, const Animations &cachedAnimations,
               Bullet::Type t, const sf::Vector2f &spawnPos,
               const sf::Vector2f &initSpeed, const sf::Vector2f &accel,
               float dmgRadius, float range, bool shotByPlayer)
    : TangibleObject(params, cachedAnimations), type(t), acceleration(accel),
      damageRadius(dmgRadius), maxRange(range), spawnPos(spawnPos),
      shotByPlayer(shotByPlayer) {

  physics.setSpeed(initSpeed, PhysicsComponent::SpeedType::MOVEMENT);
  if (type != Bullet::Type::BubbleGun) {
    physics.turnOffXFriction();
  } else {
    physics.friction.x = 0.99f;
  }

  renderizer.setColor(ColorPalette::LimeGreen);

  switch (type) {
  case Bullet::Type::Normal:
    collider.setSize({7.f, 7.f});
    collider.setOffset({4.f, 4.f});
    physics.gravity = 0.f;
    break;

  case Bullet::Type::BubbleGun:
    collider.setSize({7.f, 7.f});
    collider.setOffset({4.f, 4.f});
    physics.gravity = 0.3f;
    maxBounces = 10;
    maxLifeTime = 8.f;
    physics.setSpdy(-3.f, PhysicsComponent::SpeedType::MOVEMENT);
    break;

  case Bullet::Type::Bazooka:
    physics.gravity = 0.f;
    break;
  }

  collider.horizontalLevelCollision(position);
  collider.verticalLevelCollision(position);
}

void Bullet::update(const GeneralContext &ctx) {
  if (dead)
    return;

  if (!shotByPlayer) {
    renderizer.toggleColorEvery(0.1f, ColorPalette::White,
                                ColorPalette::MexicanPink);
  }

  if (!dying) {
    updateBehavior(ctx);
    checkLifetime(ctx);
  } else {
    animator.update();
    if (animator.animationFinished()) {
      dead = true;
    }
  }

  TangibleObject::update(ctx);
}

void Bullet::updateBehavior(const GeneralContext &ctx) {
  physics.setSpeed(physics.getSpeed(PhysicsComponent::SpeedType::MOVEMENT) +
                       acceleration,
                   PhysicsComponent::SpeedType::MOVEMENT);

  physics.updateY(position);
  if (collider.verticalLevelCollision(position)) {
    if (type == Bullet::Type::BubbleGun) {
      bounceCount++;
      physics.setSpdy(-physics.getSpdy(PhysicsComponent::SpeedType::MOVEMENT) *
                          0.9f,
                      PhysicsComponent::SpeedType::MOVEMENT);

      if (bounceCount >= maxBounces)
        die();
    } else {
      die();
    }
  }

  physics.updateX(position);
  if (collider.horizontalLevelCollision(position)) {
    if (type == Bullet::Type::BubbleGun) {
      bounceCount++;
      physics.setSpdx(-physics.getSpdx(PhysicsComponent::SpeedType::MOVEMENT) *
                          0.9f,
                      PhysicsComponent::SpeedType::MOVEMENT);

      if (bounceCount >= maxBounces)
        die();
    } else {
      die();
    }
  }
}

void Bullet::checkLifetime(const GeneralContext &ctx) {
  lifeTimer += 1.f / Constants::FRAME_RATE;

  if (maxLifeTime > 0.f && lifeTimer >= maxLifeTime)
    die();

  float dist = std::hypot(position.x - spawnPos.x, position.y - spawnPos.y);

  if (dist >= maxRange)
    die();
}

void Bullet::die() {
  if (dying)
    return;

  dying = true;
  animator.play("die_once");

  collider.setSize({damageRadius * 2.f, damageRadius * 2.f});
  collider.setOffset({-damageRadius, -damageRadius});
}

bool Bullet::isShotByPlayer() { return shotByPlayer; }