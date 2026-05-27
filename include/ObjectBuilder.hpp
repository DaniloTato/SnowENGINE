#pragma once

#include "AnimationFactory.hpp"
#include "RenderizerParameters.hpp"
#include "TextureManager.hpp"
#include "WindowManager.hpp"
#include <string>

template <typename T> class ObjectBuilder {
public:
  ObjectBuilder(const std::string &textureKey, WindowManager &windowManager)
      : position(0.f, 0.f),
        params{.windowManager = windowManager,
               .window = WindowID(),
               .texture = &TextureManager::getInstance().get(textureKey),
               .camera = nullptr,
               .layer = 0.f,
               .parallax = 1.f} {}

  ObjectBuilder &at(float x, float y) {
    position = {x, y};
    return *this;
  }

  ObjectBuilder &withEmptyAnimation(int w, int h) {
    animations = AnimationFactory::empty(w, h);
    hasAnimation = true;
    return *this;
  }

  ObjectBuilder &onCamera(GameCamera &cam) {
    params.camera = &cam;
    return *this;
  }

  ObjectBuilder &onWindow(WindowID window) {
    params.window = window;
    return *this;
  }

  T *build() {
    auto *obj = new T(params, animations);
    obj->position = position;
    if (hasAnimation) {
      obj->animator.setAnimations(animations);
      obj->animator.play("idle");
    }
    return obj;
  }

private:
  bool hasAnimation = false;
  sf::Vector2f position;
  RenderizerParameters params;
  Animations animations;
};