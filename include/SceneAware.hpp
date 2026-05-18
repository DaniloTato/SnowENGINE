#pragma once

#include <vector>

class SceneAware {
public:
  SceneAware() { registry().push_back(this); }

  virtual ~SceneAware() {
    auto &r = registry();
    std::erase(r, this);
  }

  virtual void onSceneUnload() = 0;
  virtual void onSceneLoad() {}

  static void notifySceneUnload() {
    auto copy = registry();

    for (auto *obj : copy) {
      if (obj)
        obj->onSceneUnload();
    }
  }

  SceneAware(const SceneAware &) = delete;
  SceneAware &operator=(const SceneAware &) = delete;

private:
  static std::vector<SceneAware *> &registry() {
    static std::vector<SceneAware *> r;
    return r;
  }
};