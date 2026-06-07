#include "basicMovementScript.hpp"

#include "InputManager.hpp"
#include "TangibleObject.hpp"

#include "Helpers.hpp"
#include "NamedScript.hpp" // IWYU pragma: keep
#include "RegistryMacros.hpp"
#include "ScriptRegistry.hpp" // IWYU pragma: keep
#include "Scripter.hpp"

namespace {
const float SPEED = 200.f;

struct BasicMovementState {
  Helper::DoEvery emitStarsEvery;
};
} // namespace

namespace Scripts {

void basicMovementScript(TangibleObject &tangible, const GeneralContext &ctx) {

  auto &state =
      tangible.scripter.getState<BasicMovementState>("BasicMovementScript");

  InputManager &inputManager = InputManager::getInstance();

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

  state.emitStarsEvery(1, [&tangible, &ctx] {
    ctx.particleManager->emitStars(tangible.position, 5);
  });

  tangible.physics.updateX(tangible.position);
  tangible.physics.updateY(tangible.position);
}

} // namespace Scripts

REGISTER_SCRIPT("basicMovementScript", Scripts::basicMovementScript,
                TangibleObject);