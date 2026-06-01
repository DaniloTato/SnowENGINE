#pragma once

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

  template <typename Builder>
  typename Builder::ObjectType *create(const Builder &builder);

  [[nodiscard]] const std::vector<std::unique_ptr<GameObject>> &
  getObjects() const;

  void applyDestroyQueue();

  // Dangerous. Refactor Later
  [[nodiscard]] GameObject *findObjectById(unsigned int id) const;

protected:
  std::vector<std::unique_ptr<GameObject>> objects;
};

template <typename Builder>
typename Builder::ObjectType *Scene::create(const Builder &builder) {
  auto obj = builder.create();
  auto *ptr = obj.get();
  objects.push_back(std::move(obj));
  return ptr;
}