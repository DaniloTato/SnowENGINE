#include "CameraManager.hpp"

GameCamera *CameraManager::getCamera(const CameraID &id) const {
  auto it = cameras.find(id);

  if (it == cameras.end()) {
    return nullptr;
  }

  return it->second;
}

std::vector<GameCamera *> CameraManager::getAllCameras() const {
  std::vector<GameCamera *> result;
  result.reserve(cameras.size());

  for (const auto &[id, camera] : cameras) {
    if (camera) {
      result.push_back(camera);
    }
  }

  return result;
}

CameraView CameraManager::buildView(CameraID id) const {
  if (id.isNull()) {
    return {{0.f, 0.f}, 1.f};
  }

  const GameCamera *cam = getCamera(id);

  if (!cam) {
    return {{0.f, 0.f}, 1.f};
  }

  return {cam->getPosition(), cam->getZoom()};
}

CameraID CameraManager::registerCamera(GameCamera *camera) {
  if (!camera) {
    return getInvalidId();
  }

  CameraID id{nextId++};

  // Ideally this should become camera->setCameraId(id) or rethink the ownership
  // of camId

  camera->myCamId = id;

  cameras.emplace(id, camera);

  return id;
}

void CameraManager::unregisterCamera(GameCamera *camera) {
  if (!camera) {
    return;
  }

  cameras.erase(camera->myCamId);
}