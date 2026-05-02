#pragma once
#include "TangibleObject.hpp"

class Bullet : public TangibleObject {
public:
  enum class Type : std::uint8_t { Normal, BubbleGun, Bazooka };

  Bullet(RenderizerParameters params, const Animations &cachedAnimations,
         Type type, const sf::Vector2f &spawnPos, const sf::Vector2f &initSpeed,
         const sf::Vector2f &acceleration, float damageRadius, float maxRange,
         bool shotByPlayer);

  void update(const GeneralContext &ctx) override;

  bool isDead() const { return dead; }
  bool isDying() const { return dying; }
  void die();

  bool isShotByPlayer();

private:
  void updateBehavior(const GeneralContext &ctx);
  void checkLifetime(const GeneralContext &ctx);

private:
  Type type;

  sf::Vector2f acceleration;
  float damageRadius;
  float maxRange;

  sf::Vector2f spawnPos;

  int bounceCount = 0;
  int maxBounces = 0;

  float lifeTimer = 0.f;
  float maxLifeTime = 0.f;

  bool dying = false;
  bool dead = false;

  bool shotByPlayer;
};