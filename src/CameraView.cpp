#include "CameraView.hpp"
#include "Constants.hpp"

CameraView::CameraView(WindowID targetWindow, sf::Vector2f position, float zoom,
                       std::vector<RenderCommand> &&commands)
    : targetWindow(targetWindow), position(position), zoom(zoom),
      commands(std::move(commands)) {}

float CameraView::getZoom() const { return zoom; }

sf::Vector2f CameraView::worldToScreen(const sf::Vector2f &worldPos,
                                       float paralax) const {
  if (paralax <= 0.f)
    paralax = 1.f;

  sf::Vector2f base = worldPos - (position / paralax);
  sf::Vector2f zoomOffset = (zoom - 1.f) * base;

  float paralaxOffsetX =
      ((-2 + Constants::SCREEN_WIDTH * (-1 + paralax)) / (2 * paralax) + 1);

  float paralaxOffsetY =
      ((-2 + Constants::SCREEN_HEIGHT * (-1 + paralax)) / (2 * paralax) + 1);

  sf::Vector2f paralaxOffset(paralaxOffsetX, paralaxOffsetY);

  return base + zoomOffset + paralaxOffset;
}

sf::Vector2f CameraView::screenToWorld(const sf::Vector2f &screenPos,
                                       float paralax) const {
  if (paralax <= 0.f)
    paralax = 1.f;

  float paralaxOffsetX =
      ((-2 + Constants::SCREEN_WIDTH * (-1 + paralax)) / (2 * paralax) + 1);

  float paralaxOffsetY =
      ((-2 + Constants::SCREEN_HEIGHT * (-1 + paralax)) / (2 * paralax) + 1);

  sf::Vector2f paralaxOffset(paralaxOffsetX, paralaxOffsetY);

  sf::Vector2f base = (screenPos - paralaxOffset) / zoom;

  sf::Vector2f world = base + (position / paralax);

  return world;
}

const std::vector<RenderCommand> &CameraView::getCommands() const {
  return commands;
}

WindowID CameraView::getTargetWindow() const { return targetWindow; }

void CameraView::mergeViewCommands(const CameraView &addition) {
  commands.insert(commands.end(), addition.getCommands().begin(),
                  addition.getCommands().end());
}

void CameraView::addCommands(const std::vector<RenderCommand> &toAdd) {
  commands.insert(commands.end(), toAdd.begin(), toAdd.end());
}