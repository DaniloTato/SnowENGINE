#pragma once

#include "AnimatedObject.hpp"
#include "AnimationFactory.hpp"
#include "CameraComponent.hpp"
#include "RenderizerParameters.hpp"
#include "ScriptRunner.hpp"
#include "SpriteComponent.hpp"
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
    hasSprite = true;

    params.registerAsRectShape = true;
    rectProportions = {0, 0, width, height};
    return *this;
  }

  ObjectBuilder &withTexture(std::string_view textureKey) {
    hasSprite = true;

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
    if (camera && camera->cameraComponent) {
      myCamera = camera;
    }
    return *this;
  }

  ObjectBuilder &onWindow(WindowID window) {
    hasSprite = true;

    params.window = window;
    return *this;
  }

  ObjectBuilder &layer(float l) {
    hasSprite = true;

    params.layer = l;
    return *this;
  }

  ObjectBuilder &parallax(float p) {
    hasSprite = true;

    params.parallax = p;
    return *this;
  }

  ObjectBuilder &inUpdateDomain(GameObject::UpdateDomain domain) {
    objectUpdateDomain = std::move(domain);
    return *this;
  }

  ObjectBuilder &withCameraComponent(Scene *scene, float zoom,
                                     WindowID windowDisplayedIn) {
    hasCamera = true;
    myScene = scene;
    myWindow = windowDisplayedIn;
    cameraZoom = zoom;
    return *this;
  }

  ObjectBuilder &script(Scripter<T>::ScriptFunc script) {
    scripts.push_back(script);
    return *this;
  }

  std::unique_ptr<T> create() const {

    std::unique_ptr<T> obj;

    if constexpr (std::is_same_v<T, TangibleObject>) {
      obj = std::make_unique<T>(params, animations);

    } else if constexpr (std::is_same_v<T, ScriptRunner> ||
                         std::is_same_v<T, GameCamera>) {
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

    if (myCamera) {
      myCamera->cameraComponent->subscribe(obj.getId());
    }

    if (hasCamera) {
      auto camera = std::make_unique<CameraComponent>(myScene, myWindow);

      camera->goTo(position);
      camera->zoomTo(cameraZoom);

      obj.cameraComponent = camera.release();
    }

    if (hasSprite) {
      auto sprite = std::make_unique<SpriteComponent>(params);

      if (params.registerAsRectShape) {
        sprite->setRect(rectProportions, 1);
      }

      obj.spriteComponent = sprite.release();
    }

    if constexpr (std::is_same_v<T, AnimatedObject> ||
                  std::is_same_v<T, TangibleObject>) {

      if (hasAnimation) {
        obj.animator.setAnimations(animations);
        obj.animator.play("idle");
      }
    }

    if constexpr (std::is_same_v<T, AnimatedObject> ||
                  std::is_same_v<T, ScriptRunner> ||
                  std::is_same_v<T, RenderableObject> ||
                  std::is_same_v<T, GameCamera> ||
                  std::is_same_v<T, TangibleObject>) {

      for (auto &script : scripts) {
        obj.scripter.addScript(script);
      }
    }

    if constexpr (!std::is_same_v<T, ScriptRunner>) {
      obj.setUpdateDomain(objectUpdateDomain);
    }
  }

private:
  bool hasAnimation = false;
  bool hasCamera = false;
  float cameraZoom = 1.f;
  bool hasSprite = false;
  WindowID myWindow;
  GameCamera *myCamera = nullptr;
  Scene *myScene = nullptr;
  std::vector<typename Scripter<T>::ScriptFunc> scripts;
  GameObject::UpdateDomain objectUpdateDomain;
  sf::IntRect rectProportions;
  sf::Vector2f position;
  RenderizerParameters params;
  Animations animations;
};