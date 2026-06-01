#pragma once

#include "CameraView.hpp"
#include "GameCamera.hpp"
#include <unordered_map>
#include <vector>

#include "CameraID.hpp"

// Right now, camera Manager does not have much purpose. Let's rethink that
// later.

class CameraManager {

private:
  std::unordered_map<CameraID, GameCamera *> cameras;
  size_t nextId = 1;

public:
  CameraManager() = default;
  CameraManager(const CameraManager &) = delete;
  CameraManager &operator=(const CameraManager &) = delete;
  CameraManager(CameraManager &&) = delete;
  CameraManager &operator=(CameraManager &&) = delete;

  [[nodiscard]] std::vector<GameCamera *> getAllCameras() const;

  [[nodiscard]] CameraView buildView(CameraID id) const;

  CameraID registerCamera(GameCamera *camera);
  void unregisterCamera(GameCamera *camera);

  [[nodiscard]] CameraID getInvalidId() const { return CameraID{0}; }

private:
  [[nodiscard]] GameCamera *getCamera(const CameraID &id) const;
};