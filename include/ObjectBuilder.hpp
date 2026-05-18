#pragma once

#include "AnimationFactory.hpp"
#include "RenderizerParameters.hpp"
#include "TextureManager.hpp"
#include "WindowManager.hpp"
#include <string>

template <typename T> class ObjectBuilder {
public:
  ObjectBuilder(const std::string &textureKey) {
    WindowManager &wm = WindowManager::getInstance();
    params.window = wm.getMain();
    params.texture = &TextureManager::getInstance().get(textureKey);
    params.camera = nullptr;
    params.layer = 0.f;
    params.parallax = 1.f;
  }

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
  bool hasAnimation;
  sf::Vector2f position;
  RenderizerParameters params;
  Animations animations;
};