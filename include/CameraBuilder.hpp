#include "GameObject.hpp"
#include "Scripter.hpp"

class CameraBuilder {
public:
  using ObjectType = GameCamera;
  CameraBuilder() = default;

  CameraBuilder &persistent(bool v = true);
  CameraBuilder &updateDomain(GameObject::UpdateDomain d);
  CameraBuilder &script(Scripter<GameCamera>::ScriptFunc s);
  CameraBuilder &at(float x, float y);
  CameraBuilder &zoom(float z);

  [[nodiscard]] std::unique_ptr<GameCamera> create() const;

private:
  bool isPersistent = false;
  float zoomLevel = 1.f;
  sf::Vector2f position;
  GameObject::UpdateDomain domain = WindowManager::Set::MAIN;
  Scripter<GameCamera>::ScriptFunc scriptFunc;
};
