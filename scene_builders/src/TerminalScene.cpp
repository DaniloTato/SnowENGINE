#include "TerminalScene.hpp"

#include "Constants.hpp"
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

  text = create(ObjectBuilder<GameObject>(engine)
                    .withTexture("snowFont")
                    .at(10.f, 10.f)
                    .textBoundary(880.f)
                    .onCamera(camera)
                    .layer(Constants::UI_LAYER)
                    .withText(""));
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
    if (text->textComponent) {
      text->textComponent->setMarkup(lastMarkup);
    } else {
      std::cout << "[TerminalScene] holds a text with no textComponent\n";
    }
  }
}

void TerminalScene::handleEvent(WindowID id, const sf::Event &event) {
  terminal.handleEvent(id, event);
}