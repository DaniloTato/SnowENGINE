#include "GameTextBuilder.hpp"

#include "TextureManager.hpp"

GameTextBuilder::GameTextBuilder(std::string_view fontTextureKey,
                                 Engine &engine)
    : params{.engine = engine,
             .window = WindowID(),
             .texture = &TextureManager::getInstance().get(
                 std::string(fontTextureKey)),
             .camera = nullptr,
             .layer = 0.f,
             .parallax = 1.f,
             .registerAsRectShape = false} {}

GameTextBuilder &GameTextBuilder::at(sf::Vector2f p) {
  textPosition = p;
  return *this;
}

GameTextBuilder &GameTextBuilder::onWindow(WindowID window) {
  params.window = window;
  return *this;
}

GameTextBuilder &GameTextBuilder::boundary(float b) {
  textBoundary = b;
  return *this;
}

GameTextBuilder &GameTextBuilder::typewriter(float speed) {
  useTypewriter = true;
  typewriterSpeed = speed;
  return *this;
}

GameTextBuilder &GameTextBuilder::alignment(GameText::Align a) {
  textAlignment = a;
  return *this;
}

GameTextBuilder &GameTextBuilder::markup(const std::string &m) {
  markupText = m;
  return *this;
}

GameTextBuilder &GameTextBuilder::camera(GameCamera *camera) {
  params.camera = camera;
  return *this;
}

GameTextBuilder &GameTextBuilder::layer(float l) {
  params.layer = l;
  return *this;
}

GameTextBuilder &GameTextBuilder::parallax(float p) {
  params.parallax = p;
  return *this;
}

std::unique_ptr<GameText> GameTextBuilder::create() const {
  auto text = std::make_unique<GameText>(params);
  text->setPosition(textPosition);
  text->setBoundary(textBoundary);
  text->setAlignment(textAlignment);

  if (useTypewriter) {
    text->setTypewriter(true, typewriterSpeed);
  }

  if (!markupText.empty()) {
    text->loadFromMarkup(markupText);
  }

  return text;
}