#pragma once
#include "GameObject.hpp"
#include "SFML/System/Vector2.hpp"
#include "Scripter.hpp"
#include <SFML/Graphics.hpp>

struct RenderizerParameters;

struct GeneralContext;

class GameCamera : public GameObject {
public:
  GameCamera(UpdateDomain updateDomain);

  [[nodiscard]] const sf::Vector2f &getPosition() const;
  void update(const GeneralContext &ctx) override;
  void goTo(const sf::Vector2f &pos);
  void zoomTo(float desiredZoom);
  [[nodiscard]] const sf::Vector2f getDesiredPosition() const;
  [[nodiscard]] float getZoom() const;
  [[nodiscard]] float getDesiredZoom() const;
  void setCameraShakePosition(const sf::Vector2f &shakePos);
  void setImpactZoom(float impactZoom);
  void zoomToDesired();
  void goToDesired();

  GameObjectExposure::Value::Object describe() override;

  [[nodiscard]] sf::FloatRect getWorldViewRect() const;

  Scripter<GameCamera> scripter;

private:
  sf::Vector2f desiredPosition;
  sf::Vector2f shakePosition;
  float zoom;
  float desiredZoom;
  float impactZoom;
  sf::Vector2f followSpeed;
  float zoomSpeed;
};