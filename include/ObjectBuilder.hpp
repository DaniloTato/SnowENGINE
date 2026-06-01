#pragma once

#include "AnimatedObject.hpp"
#include "AnimationFactory.hpp"
#include "RenderableObject.hpp"
#include "RenderizerParameters.hpp"
#include "ScriptRunner.hpp"
#include "TangibleObject.hpp"
#include "TextureManager.hpp"

#include <memory>
#include <type_traits>

template <typename T> class ObjectBuilder {
public:
  using ObjectType = T;

  ObjectBuilder(Engine &engine)
      : objectUpdateDomain(WindowManager::Set::MAIN), position(0.f, 0.f),
        params{.engine = engine,
               .window = WindowID(),
               .texture = nullptr,
               .camera = nullptr,
               .layer = 0.f,
               .parallax = 1.f,
               .registerAsRectShape = false} {}

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

  ObjectBuilder &onCamera(GameCamera *camera) {
    params.camera = camera;
    return *this;
  }

  ObjectBuilder &onWindow(WindowID window) {
    params.window = window;
    return *this;
  }

  ObjectBuilder &layer(float l) {
    params.layer = l;
    return *this;
  }

  ObjectBuilder &parallax(float p) {
    params.parallax = p;
    return *this;
  }

  ObjectBuilder &inUpdateDomain(GameObject::UpdateDomain domain) {
    objectUpdateDomain = std::move(domain);
    return *this;
  }

  std::unique_ptr<T> create() const {

    std::unique_ptr<T> obj;

    if constexpr (std::is_same_v<T, TangibleObject>) {
      obj = std::make_unique<T>(params, animations);

    } else if constexpr (std::is_same_v<T, ScriptRunner>) {
      obj = std::make_unique<T>(objectUpdateDomain);
    } else {
      obj = std::make_unique<T>(params);
    }

    obj->position = position;
    configure(*obj);
    return obj;
  }

private:
  void configure(T &obj) const {

    if constexpr (std::is_same_v<T, AnimatedObject> ||
                  std::is_same_v<T, TangibleObject>) {

      if (hasAnimation) {
        obj.animator.setAnimations(animations);
        obj.animator.play("idle");
      }
    }

    if constexpr (std::is_same_v<T, RenderableObject> ||
                  std::is_same_v<T, TangibleObject> ||
                  std::is_same_v<T, AnimatedObject>) {

      if (params.registerAsRectShape) {
        obj.renderizer.setRect(rectProportions, 1);
      }
    }

    if constexpr (!std::is_same_v<T, ScriptRunner>) {
      obj.setUpdateDomain(objectUpdateDomain);
    }
  }

private:
  bool hasAnimation = false;
  GameObject::UpdateDomain objectUpdateDomain;
  sf::IntRect rectProportions;
  sf::Vector2f position;
  RenderizerParameters params;
  Animations animations;
};