#include "CameraView.hpp"
#include "Constants.hpp"

CameraView::CameraView(sf::Vector2f position, float zoom)
    : position(position), zoom(zoom) {}

float CameraView::getZoom() const { return zoom; }

sf::Vector2f CameraView::worldToScreen(const sf::Vector2f &worldPos,
                                       float parallax) const {
  if (parallax <= 0.f)
    parallax = 1.f;

  sf::Vector2f base = worldPos - (position / parallax);
  sf::Vector2f zoomOffset = (zoom - 1.f) * base;

  float parallaxOffsetX =
      ((-2 + Constants::SCREEN_WIDTH * (-1 + parallax)) / (2 * parallax) + 1);

  float parallaxOffsetY =
      ((-2 + Constants::SCREEN_HEIGHT * (-1 + parallax)) / (2 * parallax) + 1);

  sf::Vector2f parallaxOffset(parallaxOffsetX, parallaxOffsetY);

  return base + zoomOffset + parallaxOffset;
}

sf::Vector2f CameraView::screenToWorld(const sf::Vector2f &screenPos,
                                       float parallax) const {
  if (parallax <= 0.f)
    parallax = 1.f;

  float parallaxOffsetX =
      ((-2 + Constants::SCREEN_WIDTH * (-1 + parallax)) / (2 * parallax) + 1);

  float parallaxOffsetY =
      ((-2 + Constants::SCREEN_HEIGHT * (-1 + parallax)) / (2 * parallax) + 1);

  sf::Vector2f parallaxOffset(parallaxOffsetX, parallaxOffsetY);

  sf::Vector2f base = (screenPos - parallaxOffset) / zoom;

  sf::Vector2f world = base + (position / parallax);

  return world;
}