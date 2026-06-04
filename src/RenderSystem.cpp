#include "RenderSystem.hpp"
#include "CameraView.hpp"
#include "Constants.hpp"
#include <iostream>

RenderSystem::RenderSystem(WindowManager &windowManager)
    : windowManager(windowManager) {}

void RenderSystem::render(const CameraView &view) {
  for (const auto &command : view.getCommands()) {
    sf::Sprite sprite;

    if (command.texture)
      sprite.setTexture(*command.texture);

    sprite.setTextureRect(command.textureRect);
    sprite.setColor(command.color);

    sprite.setPosition(view.worldToScreen(command.position, command.paralax));

    sprite.setScale(view.getZoom(), view.getZoom());

    if (!isVisible(sprite))
      continue;

    auto pos = sprite.getPosition();
    auto bounds = sprite.getGlobalBounds();
    std::cout << "pos=(" << pos.x << "," << pos.y << ") "
              << "size=(" << bounds.getSize().x << "," << bounds.getSize().y
              << ") "
              << "tex=" << command.texture << "\n";

    windowManager.drawOnWindow(command.window, sprite);
    std::cout << "rendering\n";
  }
}

bool RenderSystem::isVisible(const sf::Sprite &sprite) const {
  auto bounds = sprite.getGlobalBounds();

  return !(bounds.getPosition().x + bounds.getSize().x < 0.f ||
           bounds.getPosition().x > Constants::SCREEN_WIDTH ||
           bounds.getPosition().y + bounds.getSize().y < 0.f ||
           bounds.getPosition().y > Constants::SCREEN_HEIGHT);
}