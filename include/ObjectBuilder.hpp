#pragma once

#include "AnimatedObject.hpp"
#include "AnimationFactory.hpp"
#include "RenderableObject.hpp"
#include "RenderizerParameters.hpp"
#include "TangibleObject.hpp"
#include "TextureManager.hpp"

template <typename T> class ObjectBuilder {
public:
  ObjectBuilder(Engine &engine)
      : position(0.f, 0.f), params{.engine = engine,
                                   .window = WindowID(),
                                   .texture = nullptr,
                                   .camera = CameraID(),
                                   .layer = 0.f,
                                   .parallax = 1.f},
        engine(engine) {}

  ObjectBuilder &at(float x, float y) {
    position = {x, y};
    return *this;
  }

  ObjectBuilder &rectangle(int width, int height) {
    params.registerAsRectShape = true;
    rectProportions = {0, 0, width, height};
    return *this;
  }

  ObjectBuilder &withTexture(std::string_view textureKey) {
    params.texture = &TextureManager::getInstance().get(textureKey);
    return *this;
  }

  ObjectBuilder &withEmptyAnimation(int w, int h) {
    animations = AnimationFactory::empty(w, h);
    hasAnimation = true;
    return *this;
  }

  ObjectBuilder &onCamera(CameraID cam) {
    params.camera = cam;
    return *this;
  }

  ObjectBuilder &onWindow(WindowID window) {
    params.window = window;
    return *this;
  }

  T *build() {
    T *obj = nullptr;

    if constexpr (std::is_same_v<std::decay_t<T>, TangibleObject>) {
      obj = new T(params, animations);
    } else {
      obj = new T(params);
    }

    obj->position = position;

    if constexpr (std::is_same_v<std::decay_t<T>, AnimatedObject> ||
                  std::is_same_v<std::decay_t<T>, TangibleObject>) {
      if (hasAnimation) {
        obj->animator.setAnimations(animations);
        obj->animator.play("idle");
      }
    }

    if constexpr (std::is_same_v<std::decay_t<T>, RenderableObject> ||
                  std::is_same_v<std::decay_t<T>, TangibleObject> ||
                  std::is_same_v<std::decay_t<T>, AnimatedObject>) {
      if (params.registerAsRectShape) {
        obj->renderizer.setRect(rectProportions, 1);
      }
    }

    return obj;
  }

private:
  bool hasAnimation = false;
  // Bad field. Not always used.
  sf::IntRect rectProportions;
  sf::Vector2f position;
  RenderizerParameters params;
  Animations animations;
  Engine &engine;
};