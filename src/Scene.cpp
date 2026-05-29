#include "Scene.hpp"
#include "GameObject.hpp"

void Scene::update(const GeneralContext &ctx) {
  for (auto &obj : objects) {
    obj->update(ctx);
  }
}

template <typename T, typename... Args> T *Scene::createObject(Args &&...args) {
  static_assert(std::is_base_of_v<GameObject, T>);
  auto obj = std::make_unique<T>(std::forward<Args>(args)...);
  T *ptr = obj.get();
  objects.push_back(std::move(obj));
  return ptr;
}