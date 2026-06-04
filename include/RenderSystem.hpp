#pragma once

#include "CameraView.hpp"
#include "WindowManager.hpp"

class RenderSystem {
public:
  explicit RenderSystem(WindowManager &windowManager);

  void render(const CameraView &view);

private:
  [[nodiscard]] bool isVisible(const sf::Sprite &sprite) const;

private:
  WindowManager &windowManager;
};