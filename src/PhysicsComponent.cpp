#include "PhysicsComponent.hpp"
#include "GameState.hpp"
#include "SFML/System/Vector2.hpp"

#include <iostream>

PhysicsComponent::PhysicsComponent()
    : gravity(1100.f), friction({6.f, 6.f}), hasXFriction(true),
      hasYFriction(false) {
  speeds.resize(static_cast<std::size_t>(SpeedType::COUNT));
}

void PhysicsComponent::updateX(sf::Vector2f &position) {
  float dt = GameState::getInstance().dt();

  for (sf::Vector2f &speed : speeds) {
    if (hasXFriction) {
      speed.x -= speed.x * friction.x * dt;
    }

    position.x += speed.x * dt;
  }
}

void PhysicsComponent::updateY(sf::Vector2f &position) {
  float dt = GameState::getInstance().dt();

  for (size_t i = 0; i < speeds.size(); i++) {

    if (i == static_cast<int>(SpeedType::MOVEMENT)) {
      speeds[i].y += gravity * dt;
    }

    if (hasYFriction) {
      speeds[i].y -= speeds[i].y * friction.y * dt;
    }

    position.y += speeds[i].y * dt;

    if (dt > 0.1) {
      std::cout << dt << "\n";
    }
  }
}

void PhysicsComponent::turnOffXFriction() { hasXFriction = false; }

const sf::Vector2f &PhysicsComponent::getSpeed(SpeedType type) const {
  return speeds[static_cast<int>(type)];
}

float PhysicsComponent::getSpdx(SpeedType type) const {
  return speeds[static_cast<int>(type)].x;
}

float PhysicsComponent::getSpdy(SpeedType type) const {
  return speeds[static_cast<int>(type)].y;
}

void PhysicsComponent::setSpeed(const sf::Vector2f &newSpeed, SpeedType type) {
  speeds[static_cast<int>(type)] = newSpeed;
}

void PhysicsComponent::setSpdx(float x, SpeedType type) {
  speeds[static_cast<int>(type)].x = x;
}

void PhysicsComponent::setSpdy(float y, SpeedType type) {
  speeds[static_cast<int>(type)].y = y;
}

void PhysicsComponent::turnOnYFriction() { hasYFriction = true; }

void PhysicsComponent::turnOffYFriction() { hasYFriction = false; }

GameObjectExposure::Value::Object PhysicsComponent::describe() {
  auto desc = std::make_shared<GameObjectExposure::Descriptor>();

  desc->fields["spd"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [this] {
            return GameObjectExposure::Descriptor::describeVector2f(
                speeds[static_cast<int>(SpeedType::MOVEMENT)]);
          });

  desc->fields["friction"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [this] {
            return GameObjectExposure::Descriptor::describeVector2f(friction);
          });

  desc->fields["gravity"] = GameObjectExposure::makePublicField<float>(gravity);

  return desc;
}