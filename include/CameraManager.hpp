#pragma once

#include "CameraView.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"
#include "SceneAware.hpp"
#include <unordered_map>
#include <vector>

#include "CameraID.hpp"

class CameraManager : public SceneAware {

private:
  struct CameraEntry {
    GameCamera *camera = nullptr;
    bool persistent = false;
  };

  std::unordered_map<size_t, CameraEntry> cameras;

  std::vector<GameCamera *> destroyQueue;

  size_t nextId = 1;

public:
  CameraManager() = default;
  ~CameraManager() override;
  CameraManager(const CameraManager &) = delete;
  CameraManager &operator=(const CameraManager &) = delete;
  CameraManager(CameraManager &&) = delete;
  CameraManager &operator=(CameraManager &&) = delete;

  CameraID createCamera(GameObject::UpdateDomain updateDomain,
                        Scripter<GameCamera>::ScriptFunc script = nullptr,
                        bool persistent = false);

  [[nodiscard]] std::vector<GameCamera *> getAllCameras() const;

  void destroyCamera(const CameraID &id);
  void applyQueues();
  void clear();
  void onSceneUnload() override;

  [[nodiscard]] CameraView buildView(CameraID id) const;

  [[nodiscard]] CameraID getInvalidId() const { return CameraID{0}; }

private:
  void destroyInternal(GameCamera *camera);
  [[nodiscard]] GameCamera *getCamera(const CameraID &id) const;
};