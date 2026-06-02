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

#include "CameraBuilder.hpp"
#include "Helpers.hpp"
#include "ObjectBuilder.hpp"

void MainScene::setup(Scene::Context ctx) {

  std::cout << "mainScene setup run\n";

  Engine &engine = *ctx.engine;
  WindowManager &windowManager = engine.getWindowManager();
  WindowID mainWindow = windowManager.getMain();
  LevelManager &levelManager = engine.getLevelManager();

  GameCamera *mainCamera =
      this->create(CameraBuilder()
                       .updateDomain(WindowManager::Set::MAIN)
                       .script(Scripts::cameraBehaviourScript));

  GameCamera *uiCamera =
      this->create(CameraBuilder().updateDomain(WindowManager::Set::MAIN));

  levelManager.initializeRenderContext(engine, *this, mainWindow, mainCamera);

  levelManager.loadLevel(Helper::getPath("assets/levels/Level1.json"));

  auto *image = create(ObjectBuilder<TangibleObject>(engine)
                           .withTexture("todd")
                           .at(100, 100)
                           .onWindow(mainWindow)
                           .onCamera(mainCamera)
                           .withEmptyAnimation(16, 16));

  image->scripter.addScript(Scripts::basicMovementScript);

  this->create(ObjectBuilder<RenderableObject>(engine)
                   .rectangle(20, 20)
                   .at(200, 200)
                   .onWindow(mainWindow)
                   .onCamera(mainCamera));

  this->create(
      GameTextBuilder("snowFont", engine)
          .at({0.f, 50.f})
          .onWindow(mainWindow)
          .boundary(Constants::SCREEN_WIDTH)
          .alignment(GameText::Align::Center)
          .typewriter(0.1f)
          .camera(uiCamera)
          .markup(
              R"([anim=sin][color=yellow]Hello[/color][/anim] [anim=shake][color=white]World[/color][/anim])"));

  auto *sr = this->create(ObjectBuilder<ScriptRunner>(engine).inUpdateDomain(
      GameObject::UpdateDomain{
          {WindowManager::Set::MAIN, WindowManager::Set::TERMINAL}}));

  sr->scripter.addScript(Scripts::terminalCreationScript);

  auto *sr2 = this->create(ObjectBuilder<ScriptRunner>(engine).inUpdateDomain(
      GameObject::UpdateDomain{
          {WindowManager::Set::MAIN, WindowManager::Set::DEVUI}}));

  sr2->scripter.addScript(Scripts::tilePickerCreationScript);

  GeneralContext generalContext{.player = image,
                                .mainWindow = ctx.mainWindow,
                                .mainCamera = mainCamera,
                                .engine = &engine,
                                .gameScene = this};

  GameState::getInstance().updateGeneralContext(generalContext);
}