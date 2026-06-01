#include "Scene.hpp"

#include "GameObject.hpp"

void Scene::update(const GeneralContext &ctx) {

  for (auto &obj : objects) {
    if (!obj->isPendingDestroy()) {
      obj->update(ctx);
    }
  }

  applyDestroyQueue();
}

const std::vector<std::unique_ptr<GameObject>> &Scene::getObjects() const {
  return objects;
}

void Scene::applyDestroyQueue() {
  std::erase_if(objects, [](const auto &obj) {
    if (!obj->isPendingDestroy())
      return false;

    return true;
  });
}

GameObject *Scene::findObjectById(unsigned int id) const {
  for (const auto &obj : objects) {
    if (obj && obj->getId() == id) {
      return obj.get();
    }
  }
  return nullptr;
}