#pragma once

#include "GameText.hpp"
#include "RenderizerParameters.hpp"

#include <memory>
#include <string>

class Engine;

class GameTextBuilder {
public:
  using ObjectType = GameText;
  GameTextBuilder(std::string_view fontTextureKey, Engine &engine);
  GameTextBuilder &at(sf::Vector2f p);
  GameTextBuilder &onWindow(WindowID window);
  GameTextBuilder &boundary(float b);
  GameTextBuilder &typewriter(float speed);
  GameTextBuilder &alignment(GameText::Align a);
  GameTextBuilder &markup(const std::string &m);
  GameTextBuilder &camera(GameCamera *camera);
  GameTextBuilder &layer(float l);
  GameTextBuilder &parallax(float p);
  [[nodiscard]] std::unique_ptr<GameText> create() const;

private:
  RenderizerParameters params;
  sf::Vector2f textPosition;
  float textBoundary = 0.f;
  bool useTypewriter = false;
  float typewriterSpeed = 0.f;
  GameText::Align textAlignment = GameText::Align::Left;
  std::string markupText;
};