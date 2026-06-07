#pragma once

#include "GameObject.hpp"
#include "ParticleManager.hpp"
#include "RenderSystem.hpp"
#include "TextSystem.hpp"
#include "VectorLifecycle.hpp"
#include "WindowID.hpp"
#include <memory>
#include <vector>

class Engine;

class GameObject;
struct GeneralContext;

class Scene {
public:
  struct Context {
    Engine *engine;
    WindowID mainWindow;
  };

  virtual ~Scene() = default;

  virtual void setup(Context ctx) {}

  virtual void update(const GeneralContext &ctx);
  void render(RenderSystem &renderer);

  // Bad declaration of Template for create. Use polymorphism with a base
  // Builder class.
  template <typename Builder>
  typename Builder::ObjectType *create(const Builder &builder);

  [[nodiscard]] const std::vector<std::unique_ptr<GameObject>> &
  getObjects() const;

  [[nodiscard]] bool containsObject(GameObject::ID id) const;
  [[nodiscard]] GameObject *findObjectById(GameObject::ID id) const;

protected:
  std::vector<std::unique_ptr<GameObject>> objects;
  std::unordered_map<GameObject::ID, GameObject *> objectIndex;
  VectorLifecycle<GameObject> lifecycle;

  // Systems
  TextSystem textSystem;
  ParticleManager particleManager;
};

template <typename Builder>
typename Builder::ObjectType *Scene::create(const Builder &builder) {
  auto obj = builder.create();
  auto *ptr = obj.get();

  lifecycle.queueCreate(std::move(obj));
  objectIndex.emplace(ptr->getId(), ptr);

  return ptr;
}