#include "GameCamera.hpp"
#include "Constants.hpp"
#include "GameObject.hpp"
#include "SFML/System/Vector2.hpp"
#include "Scripter.hpp"

#include "GameState.hpp"

sf::Vector2i Vec2fTo2i(sf::Vector2f vec) {
  return {static_cast<int>(vec.x), static_cast<int>(vec.y)};
}

sf::Vector2f Vec2iTo2f(sf::Vector2i vec) {
  return {static_cast<float>(vec.x), static_cast<float>(vec.y)};
}

GameCamera::GameCamera(UpdateDomain updateDomain)
    : GameObject(std::move(updateDomain), {0, 0}), desiredPosition(0.f, 0.f),
      shakePosition(0.f, 0.f), zoom(1.f), desiredZoom(1.f), impactZoom(0.f),
      followSpeed(sf::Vector2f(0.1 * 50.f, 0.05 * 50.f)),
      zoomSpeed(0.3 * 50.f) {}

void GameCamera::goTo(const sf::Vector2f &pos) { desiredPosition = pos; }

void GameCamera::goToDesired() { position = desiredPosition; }

void GameCamera::zoomTo(float dZoom) { desiredZoom = dZoom; }

void GameCamera::zoomToDesired() { zoom = desiredZoom; }

void GameCamera::update(const GeneralContext &ctx) {
  scripter.runScripts(*this, ctx);
  float dt = GameState::getInstance().dt();
  zoom += ((desiredZoom + impactZoom) - zoom) * zoomSpeed * dt;
  position.x +=
      ((desiredPosition.x + shakePosition.x) - position.x) * followSpeed.x * dt;
  position.y +=
      ((desiredPosition.y + shakePosition.y) - position.y) * followSpeed.y * dt;
}

float GameCamera::getZoom() const { return zoom; }

float GameCamera::getDesiredZoom() const { return desiredZoom; }

sf::FloatRect GameCamera::getWorldViewRect() const {
  sf::Vector2f size = {Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT};
  sf::Vector2f topLeft = {position.x - (size.x * 0.5f),
                          position.y - (size.y * 0.5f)};
  return {topLeft, size};
}

void GameCamera::setCameraShakePosition(const sf::Vector2f &shakePos) {
  shakePosition = shakePos;
}

void GameCamera::setImpactZoom(float impactZoom) {
  this->impactZoom = impactZoom;
}

const sf::Vector2f GameCamera::getDesiredPosition() const {
  return desiredPosition;
}

GameObjectExposure::Value::Object GameCamera::describe() {

  auto desc = std::make_shared<GameObjectExposure::Descriptor>();

  desc->fields["zoom"] = GameObjectExposure::makeField<float>(
      [this]() { return getDesiredZoom(); }, [this](float v) { zoomTo(v); });

  desc->fields["scripts"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [this] { return scripter.describe(); });

  desc->fields["desired_pos"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [this] {
            return GameObjectExposure::Descriptor::describeVector2f(
                desiredPosition);
          });

  auto gameObjectDescriptions = GameObject::describe();
  desc->fields.merge(gameObjectDescriptions->fields);

  return desc;
}

const sf::Vector2f &GameCamera::getPosition() const { return position; }

CameraView GameCamera::buildView() { return {getPosition(), getZoom(), {}}; }