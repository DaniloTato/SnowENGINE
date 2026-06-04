#include "SpriteComponent.hpp"
#include "GameState.hpp"

SpriteComponent::SpriteComponent(const RenderizerParameters &params)
    : texture(params.texture), layer(params.layer), paralax(params.parallax),
      window(params.window) {

  if (texture) {
    sprite.setTexture(*texture);

    rect = sf::IntRect(0, 0, static_cast<int>(texture->getSize().x),
                       static_cast<int>(texture->getSize().y));
  }

  sprite.setTextureRect(rect);
}

void SpriteComponent::setRect(const sf::IntRect &newRect, int direction) {

  sf::IntRect directionRect = newRect;

  directionRect.left =
      newRect.left + directionRect.width * ((direction - 1) / -2);

  directionRect.width = newRect.width * direction;

  rect = directionRect;

  sprite.setTextureRect(rect);
}

const sf::IntRect &SpriteComponent::getRect() const { return rect; }

sf::Sprite &SpriteComponent::getSprite() { return sprite; }

const sf::Sprite &SpriteComponent::getSprite() const { return sprite; }

sf::Texture *SpriteComponent::getTexture() const { return texture; }

const sf::Color SpriteComponent::getColor() const { return color; }

float SpriteComponent::getLayer() const { return layer; }

float SpriteComponent::getParalax() const { return paralax; }

WindowID SpriteComponent::getWindow() const { return window; }

void SpriteComponent::setLayer(float newLayer) { layer = newLayer; }

void SpriteComponent::setColor(sf::Color newColor) {
  color = newColor;
  sprite.setColor(color);
}

void SpriteComponent::hide() { show = false; }

void SpriteComponent::showSprite() { show = true; }

bool SpriteComponent::shouldRender() const { return show; }

void SpriteComponent::toggleShowEvery(float time) {

  showCountDown -= GameState::getInstance().dt();

  if (showCountDown <= 0.f) {
    show = !show;
    showCountDown = time;
  }
}

void SpriteComponent::toggleColorEvery(float time, const sf::Color &color1,
                                       const sf::Color &color2) {

  colorCountDown -= GameState::getInstance().dt();

  if (colorCountDown < 0.f) {

    if (color == color1)
      color = color2;
    else
      color = color1;

    sprite.setColor(color);

    colorCountDown = time;
  }
}