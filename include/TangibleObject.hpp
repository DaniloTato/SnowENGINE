#pragma once
#include "Animator.hpp"
#include "BasicCollider.hpp"
#include "GameObject.hpp"
#include "PhysicsComponent.hpp"
#include "RenderizerParameters.hpp"
#include "Scripter.hpp"
#include <SFML/Audio.hpp>
#include <optional>

struct AttackHitbox {
  BasicCollider collider;
  bool active = false;
  int damage = 1;
  float remainingTime = 0.f;
};

class TangibleObject : public GameObject {
public:
  TangibleObject(RenderizerParameters params,
                 const Animations &cachedAnimations);
  void update(const GeneralContext &ctx) override;
  BasicCollider collider;
  Scripter<TangibleObject> scripter;
  Animator animator;
  PhysicsComponent physics;

  void playSound(const std::string &id, float volume = 100.f);
  [[nodiscard]] bool isPlayingAnySound() const;

  GameObjectExposure::Value::Object describe() override;

  std::optional<AttackHitbox> attackHitbox;
  int direction = 1;
  bool makesDamageTroughContact =
      true; // Horrible, but whatever. I'll change it eventually.

  std::unordered_map<std::string, sf::Sound> sounds;
};