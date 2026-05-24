#include "mainScene.hpp"

#include "GameState.hpp"
#include "LevelManager.hpp"
#include "ScriptRunner.hpp"
#include "TangibleObject.hpp"

#include "basicMovementScript.hpp"
#include "cameraBehaviourScript.hpp"
#include "terminalCreationScript.hpp"
#include "tilePickerCreationScript.hpp"

#include "SceneContext.hpp"

#include "Helpers.hpp"
#include "ObjectBuilder.hpp"

#include "RegistryMacros.hpp"
#include "SceneBuilderRegistry.hpp" // IWYU pragma: keep

namespace SceneBuilder {
void mainScene(SceneBuilder::SceneContext ctx) {

  WindowManager &windowManager = ctx.engine->getWindowManager();
  CameraManager &cameraManager = ctx.engine->getCameraManager();
  LevelManager &levelManager = ctx.engine->getLevelManager();

  CameraID mainCamera = cameraManager.createCamera(
      GameObject::UpdateDomain(WindowManager::Set::MAIN));

  cameraManager.getCamera(mainCamera)
      ->scripter.addScript(Scripts::cameraBehaviourScript);

  // This method should not exist. Must work towards refactoring levelManger.
  levelManager.initializeRenderContext(windowManager, ctx.mainWindow,
                                       cameraManager.getCamera(mainCamera));

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

  levelManager.loadLevel(Helper::getPath("assets/levels/Level1.json"));

  // add a window param in Object Builder
  // Horrible syntax for declaring camera
  auto *image =
      ObjectBuilder<TangibleObject>("todd", ctx.engine->getWindowManager())
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

  GeneralContext generalContext = {.player = image,
                                   .cameraManager = &cameraManager,
                                   .mainCamera = mainCamera,
                                   .engine = ctx.engine};
  GameState::getInstance().updateGeneralContext(generalContext);
}
} // namespace SceneBuilder

REGISTER_SCENE_BUILDER("mainScene", SceneBuilder::mainScene);