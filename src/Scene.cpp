#include "Scene.hpp"
#include "GameObject.hpp"

void Scene::update(const GeneralContext &ctx) {
  for (auto &obj : objects) {
    if (!obj->isPendingDestroy()) {
      obj->update(ctx);
    }
  }

  for (auto &obj : objects) {
    if (obj->isPendingDestroy()) {
      lifecycle.queueDestroy(obj.get());
    }
  }

  lifecycle.apply(objects);
}

const std::vector<std::unique_ptr<GameObject>> &Scene::getObjects() const {
  return objects;
}

GameObject *Scene::findObjectById(unsigned int id) const {
  for (const auto &obj : objects) {
    if (obj && obj->getId() == id) {
      return obj.get();
    }
  }
  return nullptr;
}