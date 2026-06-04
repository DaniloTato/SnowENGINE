#include "RenderSystem.hpp"
#include "CameraView.hpp"
#include "Constants.hpp"

RenderSystem::RenderSystem(WindowManager &windowManager, GameCamera *camera)
    : windowManager(windowManager), camera(camera) {}

bool RenderSystem::isVisible(const sf::Sprite &sprite) const {

  auto bounds = sprite.getGlobalBounds();

  return !(bounds.getPosition().x + bounds.getSize().x < 0.f ||
           bounds.getPosition().x > Constants::SCREEN_WIDTH ||
           bounds.getPosition().y + bounds.getSize().y < 0.f ||
           bounds.getPosition().y > Constants::SCREEN_HEIGHT);
}

void RenderSystem::render(GameObject &obj, SpriteComponent &spriteComp) {

  if (!spriteComp.shouldRender())
    return;

  auto &sprite = spriteComp.getSprite();
  sf::Vector2f worldPos = obj.position + obj.offset;
  const CameraView &view = camera->buildView();
  sf::Vector2f screenPos =
      view.worldToScreen(worldPos, spriteComp.getParalax());

  sprite.setPosition(screenPos);
  sprite.setScale(view.getZoom(), view.getZoom());

  if (!isVisible(sprite))
    return;

  windowManager.drawOnWindow(spriteComp.getWindow(), sprite);
}