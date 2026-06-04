#pragma once

#include "GameCamera.hpp"
#include "SpriteComponent.hpp"
#include "WindowManager.hpp"

class GameObject;

class RenderSystem {
public:
  RenderSystem(WindowManager &windowManager, GameCamera *camera);

  void render(GameObject &obj, SpriteComponent &spriteComp);

private:
  [[nodiscard]] bool isVisible(const sf::Sprite &sprite) const;

private:
  WindowManager &windowManager;
  GameCamera *camera;
};