#include "CameraBuilder.hpp"
#include "GameCamera.hpp"
#include <memory>

CameraBuilder &CameraBuilder::persistent(bool v) {
  isPersistent = v;
  return *this;
}

CameraBuilder &CameraBuilder::updateDomain(GameObject::UpdateDomain d) {
  domain = std::move(d);
  return *this;
}

CameraBuilder &CameraBuilder::script(Scripter<GameCamera>::ScriptFunc s) {
  scriptFunc = s;
  return *this;
}

CameraBuilder &CameraBuilder::at(float x, float y) {
  position = {x, y};
  return *this;
}

CameraBuilder &CameraBuilder::zoom(float z) {
  zoomLevel = z;
  return *this;
}

std::unique_ptr<GameCamera> CameraBuilder::create() const {

  auto cam = std::make_unique<GameCamera>(domain);

  cam->goTo(position);
  cam->zoomTo(zoomLevel);
  cam->goToDesired();
  cam->zoomToDesired();

  if (isPersistent) {
    cam->makePersistentAcrossScenes();
  }

  if (scriptFunc) {
    cam->scripter.addScript(scriptFunc);
  }

  return cam;
}