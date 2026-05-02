#pragma once
#include "SceneAware.hpp"
#include <algorithm>
#include <vector>

template <typename TObject, typename TCreateReq>
class QueuedManager : public SceneAware {
protected:
  std::vector<TObject *> objects;
  std::vector<TCreateReq> createQueue;
  std::vector<TObject *> deleteQueue;

  virtual TObject *createFromRequest(const TCreateReq &req) = 0;
  virtual void destroyObject(TObject *obj) = 0;

public:
  void applyQueues() {
    for (TObject *obj : deleteQueue) {
      auto it = std::find(objects.begin(), objects.end(), obj);
      if (it != objects.end()) {
        destroyObject(obj);
        objects.erase(it);
      }
    }
    deleteQueue.clear();

    for (const auto &req : createQueue) {
      TObject *obj = createFromRequest(req);
      if (obj)
        objects.push_back(obj);
    }
    createQueue.clear();
  }

  void onSceneUnload() override {
    for (auto *obj : objects)
      destroyObject(obj);

    objects.clear();
    createQueue.clear();
    deleteQueue.clear();
  }
};