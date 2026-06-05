#include "RenderSystem.hpp"
#include "CameraView.hpp"
#include "Constants.hpp"

RenderSystem::RenderSystem(WindowManager &windowManager)
    : windowManager(windowManager) {}

void RenderSystem::render(const CameraView &view) {
  for (const auto &command : view.getCommands()) {

    if (!isVisible(view.worldToScreen(command.position, command.paralax),
                   static_cast<sf::Vector2f>(command.textureRect.getSize()))) {
      continue;
    }

    if (!command.texture) {
      sf::RectangleShape rect;
      rect.setSize({(float)command.textureRect.width,
                    (float)command.textureRect.height});
      rect.setFillColor(command.color);
      rect.setPosition(view.worldToScreen(command.position, command.paralax));
      rect.setScale(view.getZoom(), view.getZoom());
      windowManager.drawOnWindow(view.getTargetWindow(), rect);
      continue;
    }

    sf::Sprite sprite;
    sprite.setTexture(*command.texture);

    sprite.setTextureRect(command.textureRect);
    sprite.setColor(command.color);

    sprite.setPosition(view.worldToScreen(command.position, command.paralax));

    sprite.setScale(view.getZoom(), view.getZoom());

    windowManager.drawOnWindow(view.getTargetWindow(), sprite);
  }
}

bool RenderSystem::isVisible(const sf::Vector2f &pos,
                             const sf::Vector2f &size) const {
  return !(pos.x + size.x < 0.f || pos.x > Constants::SCREEN_WIDTH ||
           pos.y + size.y < 0.f || pos.y > Constants::SCREEN_HEIGHT);
}