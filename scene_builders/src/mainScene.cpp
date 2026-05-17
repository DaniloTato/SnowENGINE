#include "mainScene.hpp"

#include "GameState.hpp"
#include "LevelManager.hpp"
#include "ScriptRunner.hpp"
#include "TangibleObject.hpp"

#include "basicMovementScript.hpp"
#include "cameraBehaviourScript.hpp"
#include "terminalCreationScript.hpp"
#include "tilePickerCreationScript.hpp"

#include "Helpers.hpp"
#include "ObjectBuilder.hpp"

#include "RegistryMacros.hpp"
#include "SceneBuilderRegistry.hpp" // IWYU pragma: keep

namespace SceneBuilder {
void mainScene() {

  GameState::getInstance().createCamera(
      CameraTypes::MAIN, GameObject::UpdateDomain(WindowManager::Set::MAIN));

  GameState::getInstance().getMainCamera()->scripter.addScript(
      Scripts::cameraBehaviourScript);

  /* Setup in case we'd like to have particles in the scene.

  RenderizerParameters polyRenderizerParams{
      .window = GameState::getInstance().getMainWindow(),
      .texture = &TextureManager::getInstance().get("particles"),
      .camera = GameState::getInstance().getMainCamera()};

  auto *pr = new PolyRenderizer(polyRenderizerParams);
  ParticleManager::getInstance().attachPolyRederizer(pr);
  ParticleManager::getInstance().loadAnimations(
      Animator::getAsepriteJSONAnimations(
          Helper::getPath("assets/animations/particles.json")));
  */

  LevelManager::getInstance().loadLevel(
      WindowManager::getInstance().getMain(),
      GameState::getInstance().getMainCamera(),
      Helper::getPath("assets/levels/Level1.json"));

  // add a window param in Object Builder
  auto *image = ObjectBuilder<TangibleObject>("todd")
                    .at(100, 100)
                    .onCamera(CameraTypes::MAIN)
                    .withEmptyAnimation(16, 16)
                    .build();

  // Change addScript to recieve the function reference, not a string.
  image->scripter.addScript(Scripts::basicMovementScript);

  // may need to refactor terminalCreation into a sytem for the engine
  ScriptRunner *sr = new ScriptRunner(GameObject::UpdateDomain(
      {WindowManager::Set::MAIN, WindowManager::Set::TERMINAL,
       WindowManager::Set::DEVUI}));
  sr->scripter.addScript(Scripts::terminalCreationScript);
  sr->scripter.addScript(Scripts::tilePickerCreationScript);

  GeneralContext ctx = {.player = image};
  GameState::getInstance().updateGeneralContext(ctx);
}
} // namespace SceneBuilder

REGISTER_SCENE_BUILDER("mainScene", SceneBuilder::mainScene);