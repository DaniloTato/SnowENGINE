#include "PolyRenderizer.hpp"
#include "Engine.hpp"
#include "GameObject.hpp"
#include "Renderizer.hpp"
#include <limits>

PolyRenderizer::PolyRenderizer(const RenderizerParameters &params)
    : Renderizer(params) {}

void PolyRenderizer::updateRenderCommands(
    std::vector<RenderCommand> &newRenderCommands) {
  renderCommands = newRenderCommands;
  // copy is created, but i dont want to get into dynamic memory for this one
}

void PolyRenderizer::render(GameObject *obj) {
  for (const RenderCommand &command : renderCommands) {
    sprite.setTextureRect(command.rect);
    sprite.setColor(command.color);

    float finalParalax = paralax;
    if (command.overrideParalax != std::numeric_limits<float>::max()) {
      finalParalax = command.overrideParalax;
    }

    sf::Vector2f screenPos;

    if (assignedCamera) {
      // screenPos = assignedCamera->worldToScreen(command.pos, finalParalax);
      const CameraView &view = assignedCamera->buildView();
      screenPos = view.worldToScreen(command.pos, finalParalax);
      sprite.setScale(view.getZoom(), view.getZoom());
    } else {
      screenPos = command.pos;
      sprite.setScale({1.f, 1.f});
    }
    sprite.setPosition(screenPos);
    engine.getWindowManager().drawOnWindow(window, sprite);
  }
}