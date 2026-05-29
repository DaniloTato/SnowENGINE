#include "mainScene.hpp"

#include "Constants.hpp"
#include "GameState.hpp"
#include "LevelManager.hpp"
#include "ScriptRunner.hpp"
#include "TangibleObject.hpp"

#include "GameTextBuilder.hpp"
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

  CameraManager &cameraManager = ctx.engine->getCameraManager();
  LevelManager &levelManager = ctx.engine->getLevelManager();

  CameraID mainCamera = cameraManager.createCamera(
      GameObject::UpdateDomain(WindowManager::Set::MAIN),
      Scripts::cameraBehaviourScript);
  CameraID uiCamera =
      cameraManager.createCamera(GameObject::UpdateDomain(ctx.mainWindow));

  // This method should not exist. Must work towards refactoring levelManger.
  levelManager.initializeRenderContext(*ctx.engine, ctx.mainWindow, mainCamera);

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

  auto *image = ObjectBuilder<TangibleObject>(*ctx.engine)
                    .withTexture("todd")
                    .at(100, 100)
                    .onWindow(ctx.mainWindow)
                    .onCamera(mainCamera)
                    .withEmptyAnimation(16, 16)
                    .build();

  // Change addScript to recieve the function reference, not a string.
  image->scripter.addScript(Scripts::basicMovementScript);

  ObjectBuilder<RenderableObject>(*ctx.engine)
      .rectangle(20, 20)
      .at(200, 200)
      .onWindow(ctx.mainWindow)
      .onCamera(mainCamera)
      .build();

  GameTextBuilder("snowFont", *ctx.engine)
      .at({0.f, 50.f})
      .onWindow(ctx.mainWindow)
      .boundary(Constants::SCREEN_WIDTH)
      .alignment(GameText::Align::Center)
      .typewriter(0.1f)
      .camera(uiCamera)
      .markup(
          R"([anim=sin][color=yellow]Hello[/color][/anim] [anim=shake][color=white]World[/color][/anim])")
      .build();

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