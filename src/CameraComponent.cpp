#include "CameraComponent.hpp"
#include "GameState.hpp"
#include "Scene.hpp"
#include "SpriteComponent.hpp"
#include <algorithm>

CameraComponent::CameraComponent(Scene *scene, WindowID targetWindow,
                                 sf::Vector2f &positionRef)
    : scene(scene), positionRef(positionRef), targetWindow(targetWindow) {}

void CameraComponent::update() {
  std::erase_if(subscriptions, [this](GameObject::ID id) {
    return !scene->containsObject(id);
  });

  float dt = GameState::getInstance().dt();
  zoom += ((desiredZoom + impactZoom) - zoom) * zoomSpeed * dt;
  positionRef.x += ((desiredPosition.x + shakePosition.x) - positionRef.x) *
                   followSpeed.x * dt;
  positionRef.y += ((desiredPosition.y + shakePosition.y) - positionRef.y) *
                   followSpeed.y * dt;
}

CameraView CameraComponent::buildView() {
  std::vector<RenderCommand> commands;

  for (auto id : subscriptions) {
    auto *obj = scene->findObjectById(id);

    if (!obj) {
      unsubscribe(id);
      continue;
    }

    for (auto &provider : obj->renderProviders) {
      if (!provider) {
        std::cout << "[CameraComponent] [buildView] caught null provider"
                  << "\n";
        continue;
      }
      // We could catch culling from here.
      provider->appendRenderCommands(commands);

      if (linkedParticleManager) {
        linkedParticleManager->particleRenderComponent.appendRenderCommands(
            commands);
      }
    }
  }

  std::ranges::stable_sort(commands, std::greater<float>{},
                           &RenderCommand::layer);

  return {targetWindow, positionRef, zoom, std::move(commands)};
}

void CameraComponent::subscribe(GameObject::ID id) {
  if (std::ranges::find(subscriptions, id) == subscriptions.end()) {
    subscriptions.push_back(id);
  }
}

void CameraComponent::unsubscribe(GameObject::ID id) {
  std::erase(subscriptions, id);
}

void CameraComponent::linkParticleManager(ParticleManager &particleManager) {
  linkedParticleManager = &particleManager;
}

void CameraComponent::goTo(const sf::Vector2f &pos) { desiredPosition = pos; }

void CameraComponent::goToDesired() { positionRef = desiredPosition; }

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

const sf::Vector2f &CameraComponent::getPosition() const { return positionRef; }

const sf::Vector2f &CameraComponent::getDesiredPosition() const {
  return desiredPosition;
}