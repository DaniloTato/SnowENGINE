#include "mainScene.hpp"

#include "Constants.hpp"
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

void MainScene::setup(Scene::Context ctx) {

  Engine &engine = *ctx.engine;
  WindowManager &windowManager = engine.getWindowManager();
  WindowID mainWindow = windowManager.getMain();
  LevelManager &levelManager = engine.getLevelManager();

  particleManager.loadAnimations(Animator::getAsepriteJSONAnimations(
      Helper::getPath("assets/animations/particles.json")));

  ScriptRunner *mainCameraObject =
      create(ObjectBuilder<ScriptRunner>(engine)
                 .withCameraComponent(this, 1.f, mainWindow)
                 .inUpdateDomain(WindowManager::Set::MAIN)
                 .script(Scripts::cameraBehaviourScript));
  if (mainCameraObject->cameraComponent) {
    mainCameraObject->cameraComponent->linkParticleManager(particleManager);
  }

  ScriptRunner *uiCameraObject =
      create(ObjectBuilder<ScriptRunner>(engine)
                 .withCameraComponent(this, 1.f, mainWindow)
                 .inUpdateDomain(WindowManager::Set::MAIN));

  levelManager.initializeRenderContext(engine, *this, mainWindow,
                                       mainCameraObject->cameraComponent);

  levelManager.loadLevel(Helper::getPath("assets/levels/Level1.json"));

  auto *image = create(ObjectBuilder<TangibleObject>(engine)
                           .withTexture("todd")
                           .withSprite()
                           .at(100, 100)
                           .onCamera(mainCameraObject->cameraComponent)
                           .withEmptyAnimation(16, 16));

  image->scripter.addScript(Scripts::basicMovementScript);

  create(ObjectBuilder<GameObject>(engine)
             .rectangle(20, 20)
             .withSprite()
             .at(200, 200)
             .onCamera(mainCameraObject->cameraComponent));

  create(
      ObjectBuilder<GameObject>(engine)
          .at(0.f, 50.f)
          .withTexture("snowFont")
          .withText(
              R"([anim=sin][color=yellow]Hello[/color][/anim] [anim=shake][color=white]World[/color][/anim])")
          .textBoundary(Constants::SCREEN_WIDTH)
          .textAlignment(TextAlign::Center)
          .textTypewriter(0.1f)
          .onCamera(uiCameraObject->cameraComponent));

  auto *sr = create(ObjectBuilder<ScriptRunner>(engine).inUpdateDomain(
      GameObject::UpdateDomain{GameObject::UpdateDomain::Universal}));

  sr->scripter.addScript(Scripts::terminalCreationScript);

  auto *sr2 = this->create(ObjectBuilder<ScriptRunner>(engine).inUpdateDomain(
      GameObject::UpdateDomain::Universal));

  sr2->scripter.addScript(Scripts::tilePickerCreationScript);

  GeneralContext generalContext{.player = image,
                                .mainWindow = ctx.mainWindow,
                                .particleManager = &particleManager,
                                .mainCamera = mainCameraObject->cameraComponent,
                                .engine = &engine,
                                .gameScene = this};

  GameState::getInstance().updateGeneralContext(generalContext);
}