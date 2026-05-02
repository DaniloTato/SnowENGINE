#pragma once
#include "GameObjectExposure.hpp"
#include <SFML/System/Vector2.hpp>
#include <vector>

class PhysicsComponent {
public:
  enum class SpeedType : std::uint8_t { MOVEMENT, KICK, COUNT };

  PhysicsComponent();
  PhysicsComponent(float gravity);

  void updateX(sf::Vector2f &position);
  void updateY(sf::Vector2f &position);

  [[nodiscard]] const sf::Vector2f &getSpeed(SpeedType type) const;
  [[nodiscard]] float getSpdx(SpeedType type) const;
  [[nodiscard]] float getSpdy(SpeedType type) const;

  void setSpeed(const sf::Vector2f &newSpeed, SpeedType type);
  void setSpdx(float x, SpeedType type);
  void setSpdy(float y, SpeedType type);

  void turnOffXFriction();
  void turnOnYFriction();
  void turnOffYFriction();

  GameObjectExposure::Value::Object describe();

  float gravity;
  sf::Vector2f friction;

private:
  std::vector<sf::Vector2f> speeds;
  bool hasXFriction;
  bool hasYFriction;
};