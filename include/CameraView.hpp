#pragma once

#include "Constants.hpp"
#include <SFML/System/Vector2.hpp>

class CameraView {

  sf::Vector2f position;
  float zoom = 1.f;

public:
  CameraView(sf::Vector2f position, float zoom)
      : position(position), zoom(zoom) {}

  [[nodiscard]] float getZoom() const { return zoom; }

  [[nodiscard]] sf::Vector2f worldToScreen(const sf::Vector2f &worldPos,
                                           float parallax) const {
    if (parallax <= 0.f)
      parallax = 1.f;
    sf::Vector2f base = worldPos - (position / parallax);
    sf::Vector2f zoomOffset = (zoom - 1.f) * base;
    float parallaxOffsetX =
        ((-2 + Constants::SCREEN_WIDTH * (-1 + parallax)) / (2 * parallax) + 1);
    float parallaxOffsetY =
        ((-2 + Constants::SCREEN_HEIGHT * (-1 + parallax)) / (2 * parallax) +
         1);
    sf::Vector2f parallaxOffset(parallaxOffsetX, parallaxOffsetY);
    return base + zoomOffset + parallaxOffset;
  }

  [[nodiscard]] sf::Vector2f screenToWorld(const sf::Vector2f &screenPos,
                                           float parallax) const {
    if (parallax <= 0.f)
      parallax = 1.f;

    float parallaxOffsetX =
        ((-2 + Constants::SCREEN_WIDTH * (-1 + parallax)) / (2 * parallax) + 1);
    float parallaxOffsetY =
        ((-2 + Constants::SCREEN_HEIGHT * (-1 + parallax)) / (2 * parallax) +
         1);
    sf::Vector2f parallaxOffset(parallaxOffsetX, parallaxOffsetY);

    sf::Vector2f base = (screenPos - parallaxOffset) / zoom;

    sf::Vector2f world = base + (position / parallax);

    return world;
  }
};