#pragma once

#include "RenderizerParameters.hpp"
#include <SFML/Graphics.hpp>

class SpriteComponent {
public:
  explicit SpriteComponent(const RenderizerParameters &params);

  void setRect(const sf::IntRect &newRect, int direction);

  [[nodiscard]] const sf::IntRect &getRect() const;
  [[nodiscard]] sf::Sprite &getSprite();
  [[nodiscard]] const sf::Sprite &getSprite() const;

  [[nodiscard]] float getLayer() const;
  void setLayer(float newLayer);

  void setColor(sf::Color newColor);

  void toggleShowEvery(float time);

  void toggleColorEvery(float time, const sf::Color &color1,
                        const sf::Color &color2);

  [[nodiscard]] WindowID getWindow() const;

  void hide();
  void showSprite();

  [[nodiscard]] bool shouldRender() const;

private:
  sf::Texture *texture = nullptr;
  sf::Sprite sprite;

  sf::IntRect rect;

  sf::Color color = sf::Color::White;

  float layer;
  float paralax;

  bool show = true;

  float showCountDown = 0.f;
  float colorCountDown = 0.f;

  WindowID window;
};