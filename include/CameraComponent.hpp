#pragma once

#include "CameraView.hpp"
#include "GameObject.hpp"
#include "ParticleManager.hpp"

#include <vector>

class Scene;
class GameObject;

class CameraComponent {
public:
  explicit CameraComponent(Scene *scene, WindowID targetWindow,
                           sf::Vector2f &positionRef);

  void subscribe(GameObject::ID id);
  void unsubscribe(GameObject::ID id);
  void linkParticleManager(ParticleManager &particleManager);

  void update();

  [[nodiscard]] CameraView buildView();

  void goTo(const sf::Vector2f &pos);
  void goToDesired();
  void zoomTo(float zoom);
  void zoomToDesired();
  void setCameraShakePosition(const sf::Vector2f &shakePos);
  void setImpactZoom(float impactZoom);
  [[nodiscard]] float getZoom() const;
  [[nodiscard]] float getDesiredZoom() const;
  [[nodiscard]] const sf::Vector2f &getPosition() const;
  [[nodiscard]] const sf::Vector2f &getDesiredPosition() const;

private:
  Scene *scene;
  sf::Vector2f &positionRef;
  sf::Vector2f desiredPosition;
  sf::Vector2f shakePosition;
  float zoom = 1.f;
  float desiredZoom = 1.f;
  float impactZoom = 0.f;
  sf::Vector2f followSpeed = {5.f, 2.5f};
  float zoomSpeed = 15.f;
  WindowID targetWindow;
  std::vector<GameObject::ID> subscriptions;
  ParticleManager *linkedParticleManager;
};