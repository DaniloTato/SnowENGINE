#include "TerminalScene.hpp"

#include "Constants.hpp"
#include "GameTextBuilder.hpp"
#include "GeneralContext.hpp"
#include "ObjectBuilder.hpp"

void TerminalScene::setup(Scene::Context ctx) {
  Engine &engine = *ctx.engine;
  auto &windowManager = engine.getWindowManager();

  terminalWindow = windowManager.create(WindowManager::Set::TERMINAL, 900, 500,
                                        "Snowgun Terminal");
  windowManager.subscribe(terminalWindow, this);

  camera = create(ObjectBuilder<GameObject>(engine)
                      .withCameraComponent(this, 1.f, terminalWindow)
                      .inUpdateDomain(terminalWindow))
               ->cameraComponent;

  text = create(GameTextBuilder("snowFont", engine)
                    .at({10.f, 10.f})
                    .boundary(880.f)
                    .camera(camera)
                    .layer(Constants::UI_LAYER)
                    .markup(""));
}

void TerminalScene::update(const GeneralContext &ctx) {
  Scene::update(ctx);
  terminal.update();
  if (!terminal.isOpen()) {
    auto &wm = ctx.engine->getWindowManager();
    wm.queueDestroy(terminalWindow);
    ctx.engine->getSceneManager().closeScene("terminal");
    return;
  }

  if (lastMarkup != terminal.buildMarkup()) {
    lastMarkup = terminal.buildMarkup();
    text->loadFromMarkup(lastMarkup);
  }
}

void TerminalScene::handleEvent(WindowID id, const sf::Event &event) {
  terminal.handleEvent(id, event);
}