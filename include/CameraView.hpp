#pragma once

#include <SFML/System/Vector2.hpp>

class CameraView {
private:
  sf::Vector2f position;
  float zoom = 1.f;

public:
  CameraView();
  CameraView(sf::Vector2f position, float zoom);
  [[nodiscard]] float getZoom() const;
  [[nodiscard]] sf::Vector2f worldToScreen(const sf::Vector2f &worldPos,
                                           float parallax) const;
  [[nodiscard]] sf::Vector2f screenToWorld(const sf::Vector2f &screenPos,
                                           float parallax) const;
};