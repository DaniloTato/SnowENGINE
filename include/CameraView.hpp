#pragma once

#include "RenderCommand.hpp"
#include "WindowID.hpp"
#include <SFML/System/Vector2.hpp>

class CameraView {
private:
  WindowID targetWindow;
  sf::Vector2f position;
  float zoom = 1.f;

  std::vector<RenderCommand> commands;

public:
  CameraView();
  CameraView(WindowID targetWindow, sf::Vector2f position, float zoom,
             std::vector<RenderCommand> &&commands);
  [[nodiscard]] float getZoom() const;
  [[nodiscard]] const std::vector<RenderCommand> &getCommands() const;
  [[nodiscard]] sf::Vector2f worldToScreen(const sf::Vector2f &worldPos,
                                           float paralax) const;
  [[nodiscard]] sf::Vector2f screenToWorld(const sf::Vector2f &screenPos,
                                           float paralax) const;
  [[nodiscard]] WindowID getTargetWindow() const;
};