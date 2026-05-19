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
void mainScene(Engine &engine) {

  CameraManager &cameraManager = engine.getCameraManager();

  CameraID mainCamera = cameraManager.createCamera(
      GameObject::UpdateDomain(WindowManager::Set::MAIN));

  cameraManager.getCamera(mainCamera)
      ->scripter.addScript(Scripts::cameraBehaviourScript);

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
      engine.getWindowManager(), engine.getWindowManager().getMain(),
      cameraManager.getCamera(mainCamera),
      Helper::getPath("assets/levels/Level1.json"));

  // add a window param in Object Builder
  // Horrible syntax for declaring camera
  auto *image = ObjectBuilder<TangibleObject>("todd", engine.getWindowManager())
                    .at(100, 100)
                    .onCamera(*cameraManager.getCamera(mainCamera))
                    .withEmptyAnimation(16, 16)
                    .build();

  // Change addScript to recieve the function reference, not a string.
  image->scripter.addScript(Scripts::basicMovementScript);

  // may need to refactor terminalCreation into a sytem for the engine
  ScriptRunner *sr = new ScriptRunner(GameObject::UpdateDomain(
      {WindowManager::Set::MAIN, WindowManager::Set::TERMINAL}));
  sr->scripter.addScript(Scripts::terminalCreationScript);

  ScriptRunner *sr2 = new ScriptRunner(GameObject::UpdateDomain(
      {WindowManager::Set::MAIN, WindowManager::Set::DEVUI}));
  sr2->scripter.addScript(Scripts::tilePickerCreationScript);

  GeneralContext ctx = {.player = image,
                        .cameraManager = &cameraManager,
                        .mainCamera = mainCamera,
                        .engine = &engine};
  GameState::getInstance().updateGeneralContext(ctx);
}
} // namespace SceneBuilder

REGISTER_SCENE_BUILDER("mainScene", SceneBuilder::mainScene);