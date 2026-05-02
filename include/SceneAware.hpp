#pragma once

class SceneAware {
public:
  virtual void onSceneUnload() = 0;
  virtual void onSceneLoad() {}
  virtual ~SceneAware() = default;
};