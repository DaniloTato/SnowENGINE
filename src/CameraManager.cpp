#include "CameraManager.hpp"

CameraManager::~CameraManager() { clear(); }

CameraID CameraManager::createCamera(GameObject::UpdateDomain updateDomain,
                                     bool persistent) {
  uint64_t id = nextId++;

  GameCamera *cam = new GameCamera(std::move(updateDomain));

  cameras[id] = CameraEntry{.camera = cam, .persistent = persistent};

  return CameraID{id};
}

GameCamera *CameraManager::getCamera(const CameraID &id) const {
  auto it = cameras.find(id.value);
  if (it == cameras.end() || !it->second.camera) {
    return nullptr;
  }
  return it->second.camera;
}

std::vector<GameCamera *> CameraManager::getAllCameras() const {
  std::vector<GameCamera *> result;
  result.reserve(cameras.size());

  for (const auto &pair : cameras) {
    if (pair.second.camera)
      result.push_back(pair.second.camera);
  }

  return result;
}

void CameraManager::destroyCamera(const CameraID &id) {
  auto it = cameras.find(id.value);
  if (it != cameras.end()) {
    destroyQueue.push_back(it->second.camera);
    cameras.erase(it);
  }
}

void CameraManager::applyQueues() {
  for (GameCamera *cam : destroyQueue) {
    destroyInternal(cam);
  }
  destroyQueue.clear();
}

void CameraManager::destroyInternal(GameCamera *camera) {
  if (!camera)
    return;
  GameObject::destroy(camera);
}

void CameraManager::clear() {
  for (auto &pair : cameras) {
    if (pair.second.camera && !pair.second.persistent) {
      GameObject::destroy(pair.second.camera);
    }
  }

  cameras.clear();
  destroyQueue.clear();
}

void CameraManager::onSceneUnload() {
  std::cout << "camera manager scene unload " << "\n";
  for (auto it = cameras.begin(); it != cameras.end();) {
    if (!it->second.persistent) {
      std::cout << "deleted camera " << it->first << "\n";
      destroyInternal(it->second.camera);
      it = cameras.erase(it);
    } else {
      ++it;
    }
  }

  destroyQueue.clear();
}