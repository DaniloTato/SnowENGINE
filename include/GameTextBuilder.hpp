#pragma once

#include "GameText.hpp"

class GameTextBuilder {
public:
  GameTextBuilder(std::string_view fontTextureKey, WindowManager &windowManager,
                  WindowID window);
  GameTextBuilder &at(sf::Vector2f p);
  GameTextBuilder &onWindow(WindowID window);
  GameTextBuilder &boundary(float b);
  GameTextBuilder &typewriter(float speed);
  GameTextBuilder &alignment(GameText::Align a);
  GameTextBuilder &markup(const std::string &m);
  GameTextBuilder &camera(GameCamera &cam);
  GameTextBuilder &layer(float l);
  GameTextBuilder &parallax(float p);
  GameText *build();

private:
  RenderizerParameters params;
  std::string markupText;
  sf::Vector2f textPosition = {0.f, 0.f};
  float textBoundary = 100000.f;
  bool useTypewriter = false;
  float typewriterSpeed = 0.04f;
  GameText::Align textAlignment = GameText::Align::Left;
};