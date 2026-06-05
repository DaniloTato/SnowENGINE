#pragma once

#include "CameraView.hpp"
#include "WindowManager.hpp"

class RenderSystem {
public:
  explicit RenderSystem(WindowManager &windowManager);

  void render(const CameraView &view);

private:
  [[nodiscard]] bool isVisible(const sf::Vector2f &pos,
                               const sf::Vector2f &size) const;

private:
  WindowManager &windowManager;
};