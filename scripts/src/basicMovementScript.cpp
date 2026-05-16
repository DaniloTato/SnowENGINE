#include "basicMovementScript.hpp"

#include "InputManager.hpp"
#include "TangibleObject.hpp"

#include "RegistryMacros.hpp"
#include "ScriptRegistry.hpp"
#include "Scripter.hpp"

namespace Scripts {

namespace {
const float SPEED = 200.f;
}

void basicMovementScript(TangibleObject &tangible, const GeneralContext &ctx) {

  InputManager &inputManager = InputManager::getInstance();

  // dt should be changed to be part of the context and not called by the
  // singleton.

  if (inputManager.isPressed("right")) {
    tangible.physics.setSpdx(SPEED, PhysicsComponent::SpeedType::MOVEMENT);
  } else if (inputManager.isPressed("left")) {
    tangible.physics.setSpdx(-SPEED, PhysicsComponent::SpeedType::MOVEMENT);
  }

  if (inputManager.isPressed("up")) {
    tangible.physics.setSpdy(-SPEED, PhysicsComponent::SpeedType::MOVEMENT);
  } else if (inputManager.isPressed("down")) {
    tangible.physics.setSpdy(SPEED, PhysicsComponent::SpeedType::MOVEMENT);
  }

  tangible.physics.updateX(tangible.position);
  tangible.physics.updateY(tangible.position);
}

} // namespace Scripts

REGISTER_SCRIPT("basicMovementScript", Scripts::basicMovementScript,
                TangibleObject);