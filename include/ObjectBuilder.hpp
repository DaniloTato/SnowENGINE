#pragma once

#include "AnimatedObject.hpp"
#include "AnimationFactory.hpp"
#include "CameraComponent.hpp"
#include "RenderizerParameters.hpp"
#include "ScriptRunner.hpp"
#include "SpriteComponent.hpp"
#include "TangibleObject.hpp"
#include "TextComponent.hpp"
#include "TextureManager.hpp"

#include "Constants.hpp"
#include "FontAtlas.hpp"
#include "TextProperties.hpp"

#include <memory>
#include <type_traits>

template <typename T> class ObjectBuilder {
public:
  using ObjectType = T;

  ObjectBuilder(Engine &engine)
      : objectUpdateDomain(WindowManager::Set::MAIN), position(0.f, 0.f),
        params{.engine = engine,
               .texture = nullptr,
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
    wasRectModified = true;
    return *this;
  }

  ObjectBuilder &withTextureRect(const sf::IntRect &rect) {
    rectProportions = rect;
    wasRectModified = true;
    return *this;
  }

  ObjectBuilder &withText(std::string markup) {
    hasText = true;
    myTextMarkup = std::move(markup);
    return *this;
  }

  ObjectBuilder &textBoundary(float b) {
    hasText = true;
    myTextBoundary = b;
    return *this;
  }

  ObjectBuilder &textAlignment(TextAlign a) {
    hasText = true;
    myTextAlignment = a;
    return *this;
  }

  ObjectBuilder &textTypewriter(float speed) {
    hasText = true;
    useTypewriter = true;
    textTypewriterSpeed = speed;
    return *this;
  }

  ObjectBuilder &textFont(FontAtlas atlas) {
    hasText = true;
    myTextFontAtlas = atlas;
    return *this;
  }

  ObjectBuilder &withTexture(std::string_view textureKey) {
    params.texture = &TextureManager::getInstance().get(textureKey);
    return *this;
  }

  ObjectBuilder &withTexture(sf::Texture &texture) {
    params.texture = &texture;
    return *this;
  }

  ObjectBuilder &withEmptyAnimation(int w, int h) {
    animations = AnimationFactory::empty(w, h);
    hasAnimation = true;
    return *this;
  }

  ObjectBuilder &onCamera(CameraComponent *camera) {
    myCamera = camera;
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

  ObjectBuilder &withSprite() {
    hasSprite = true;
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

    } else if constexpr (std::is_same_v<T, ScriptRunner>) {
      obj = std::make_unique<T>(objectUpdateDomain);
    } else if constexpr (std::is_same_v<T, GameObject>) {
      obj = std::make_unique<T>(objectUpdateDomain, position);
    } else {
      obj = std::make_unique<T>(params);
    }

    obj->position = position;
    configure(*obj);
    return obj;
  }

private:
  void configure(T &obj) const {

    if (hasText) {
      auto textComponent = std::make_unique<TextComponent>();

      textComponent->markup = myTextMarkup;
      textComponent->fontAtlas = myTextFontAtlas;
      textComponent->alignment = myTextAlignment;
      textComponent->boundary = myTextBoundary;

      if (useTypewriter) {
        textComponent->effect = TextEffect::Typewriter;

        textComponent->typeDelay = textTypewriterSpeed;
      }

      auto renderComponent =
          std::make_unique<TextRenderComponent>(&obj, params.texture);

      obj.textComponent = textComponent.release();

      obj.textRenderComponent = renderComponent.release();

      obj.renderProviders.push_back(obj.textRenderComponent);
    }

    if (myCamera) {
      myCamera->subscribe(obj.getId());
    }

    if (hasCamera) {
      auto camera =
          std::make_unique<CameraComponent>(myScene, myWindow, obj.position);

      camera->goTo(position);
      camera->zoomTo(cameraZoom);

      obj.cameraComponent = camera.release();
    }

    if (hasSprite) {
      auto sprite = std::make_unique<SpriteComponent>(&obj, params);

      if (wasRectModified) {
        sprite->setRect(rectProportions, 1);
      }
      obj.spriteComponent = sprite.release();
      obj.renderProviders.push_back(obj.spriteComponent);
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
  CameraComponent *myCamera = nullptr;
  Scene *myScene = nullptr;
  std::vector<typename Scripter<T>::ScriptFunc> scripts;
  GameObject::UpdateDomain objectUpdateDomain;
  sf::IntRect rectProportions;
  bool wasRectModified = false;
  sf::Vector2f position;
  RenderizerParameters params;
  Animations animations;

  // bs text related stuff

  bool hasText = false;
  std::string myTextMarkup;
  FontAtlas myTextFontAtlas = Constants::DEFAULT_FONT_ATLAS;
  TextAlign myTextAlignment = TextAlign::Left;
  float myTextBoundary = 100000.f;
  bool useTypewriter = false;
  float textTypewriterSpeed = 0.04f;
};