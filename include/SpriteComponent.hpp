#pragma once

#include "RenderProvider.hpp"
#include "RenderizerParameters.hpp"
#include <SFML/Graphics.hpp>

class GameObject;

class SpriteComponent : public RenderProvider {
public:
  explicit SpriteComponent(GameObject *owner,
                           const RenderizerParameters &params);

  void appendRenderCommands(std::vector<RenderCommand> &out) const override;

  void setRect(const sf::IntRect &newRect, int direction);

  [[nodiscard]] const sf::IntRect &getRect() const;
  [[nodiscard]] sf::Sprite &getSprite();
  [[nodiscard]] const sf::Sprite &getSprite() const;
  [[nodiscard]] sf::Texture *getTexture() const;
  [[nodiscard]] const sf::Color getColor() const;

  [[nodiscard]] float getLayer() const;
  [[nodiscard]] float getParalax() const;
  void setLayer(float newLayer);

  void setColor(sf::Color newColor);

  void toggleShowEvery(float time);

  void toggleColorEvery(float time, const sf::Color &color1,
                        const sf::Color &color2);

  void hide();
  void showSprite();

  [[nodiscard]] bool shouldRender() const;

private:
  GameObject *owner;
  sf::Texture *texture = nullptr;
  sf::Sprite sprite;

  sf::IntRect rect;

  sf::Color color = sf::Color::White;

  float layer;
  float paralax;

  bool show = true;

  float showCountDown = 0.f;
  float colorCountDown = 0.f;
};