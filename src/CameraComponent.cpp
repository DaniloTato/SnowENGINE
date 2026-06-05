#include "CameraComponent.hpp"
#include "GameState.hpp"
#include "Scene.hpp"
#include "SpriteComponent.hpp"
#include <algorithm>

CameraComponent::CameraComponent(Scene *scene, WindowID windowDisplayedIn)
    : scene(scene), windowDisplayedIn(windowDisplayedIn) {}

void CameraComponent::update() {
  std::erase_if(subscriptions, [this](GameObject::ID id) {
    return !scene->containsObject(id);
  });

  float dt = GameState::getInstance().dt();
  zoom += ((desiredZoom + impactZoom) - zoom) * zoomSpeed * dt;
  position.x +=
      ((desiredPosition.x + shakePosition.x) - position.x) * followSpeed.x * dt;
  position.y +=
      ((desiredPosition.y + shakePosition.y) - position.y) * followSpeed.y * dt;
}

CameraView CameraComponent::buildView() {
  std::vector<RenderCommand> commands;

  for (auto id : subscriptions) {
    auto *obj = scene->findObjectById(id);

    if (!obj) {
      unsubscribe(id);
      continue;
    }

    if (!obj->spriteComponent)
      continue;

    auto *sprite = obj->spriteComponent;

    if (!sprite->shouldRender())
      continue;

    sf::Vector2f renderCommandPos = obj->position + obj->offset;

    commands.push_back({.texture = sprite->getTexture(),
                        .position = renderCommandPos,
                        .textureRect = sprite->getRect(),
                        .color = sprite->getColor(),
                        .scale = {zoom, zoom},
                        .layer = sprite->getLayer(),
                        .paralax = sprite->getParalax(),
                        .window = sprite->getWindow()});
  }

  std::ranges::stable_sort(commands, std::greater<float>{},
                           &RenderCommand::layer);

  std::cout << "commands size: " << commands.size() << "\n";

  return {position, zoom, std::move(commands)};
}

void CameraComponent::subscribe(GameObject::ID id) {
  if (std::ranges::find(subscriptions, id) == subscriptions.end()) {
    subscriptions.push_back(id);
  }
}

void CameraComponent::unsubscribe(GameObject::ID id) {
  std::erase(subscriptions, id);
}

void CameraComponent::goTo(const sf::Vector2f &pos) { desiredPosition = pos; }

void CameraComponent::goToDesired() { position = desiredPosition; }

void CameraComponent::zoomTo(float newZoom) { desiredZoom = newZoom; }

void CameraComponent::zoomToDesired() { zoom = desiredZoom; }

void CameraComponent::setCameraShakePosition(const sf::Vector2f &shakePos) {
  shakePosition = shakePos;
}

void CameraComponent::setImpactZoom(float newImpactZoom) {
  impactZoom = newImpactZoom;
}

float CameraComponent::getZoom() const { return zoom; }

float CameraComponent::getDesiredZoom() const { return desiredZoom; }

const sf::Vector2f &CameraComponent::getPosition() const { return position; }

const sf::Vector2f &CameraComponent::getDesiredPosition() const {
  return desiredPosition;
}