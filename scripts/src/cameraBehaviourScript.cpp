#include "cameraBehaviourScript.hpp"

#include "CameraComponent.hpp"
#include "Constants.hpp"
#include "GameCamera.hpp"

#include "RegistryMacros.hpp"
#include "ScriptRegistry.hpp" // IWYU pragma: keep
#include "Scripter.hpp"

namespace Scripts {

void cameraBehaviourScript(GameCamera &obj, const GeneralContext &ctx) {

  if (!obj.cameraComponent)
    return;

  CameraComponent &camera = *obj.cameraComponent;

  sf::Vector2f playerPosition = ctx.player->position;

  float zoom = camera.getZoom();
  sf::Vector2f halfViewSize(Constants::SCREEN_WIDTH / 2.f / zoom,
                            Constants::SCREEN_HEIGHT / 2.f / zoom);
  sf::Vector2f desiredCamPos = playerPosition - halfViewSize;

  camera.goTo(desiredCamPos);
}

} // namespace Scripts

REGISTER_SCRIPT("cameraBehaviourScript", Scripts::cameraBehaviourScript,
                GameCamera);