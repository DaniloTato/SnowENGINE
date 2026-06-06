#include "Scene.hpp"
#include "CameraComponent.hpp"
#include "GameObject.hpp"
#include "GameState.hpp"
#include "GeneralContext.hpp"

void Scene::update(const GeneralContext &ctx) {

  for (auto &obj : objects) {
    if (!obj->isPendingDestroy()) {
      obj->update(ctx);
    }
  }

  for (auto &obj : objects) {
    if (obj->textComponent && obj->textRenderComponent) {
      textSystem.update(*obj->textComponent, *obj->textRenderComponent,
                        GameState::getInstance().dt());
    }
  }

  for (auto &obj : objects) {
    if (obj->cameraComponent) {
      obj->cameraComponent->update();
    }
  }

  for (auto &obj : objects) {
    if (obj->isPendingDestroy()) {
      lifecycle.queueDestroy(obj.get());
      objectIndex.erase(obj->getId());
    }
  }

  lifecycle.apply(objects);
}

void Scene::render(RenderSystem &renderer) {
  for (const auto &obj : objects) {
    if (!obj->cameraComponent)
      continue;
    CameraView view = obj->cameraComponent->buildView();
    renderer.render(view);
  }
}

const std::vector<std::unique_ptr<GameObject>> &Scene::getObjects() const {
  return objects;
}

bool Scene::containsObject(GameObject::ID id) const {
  return objectIndex.contains(id);
}

GameObject *Scene::findObjectById(GameObject::ID id) const {
  auto it = objectIndex.find(id);
  if (it == objectIndex.end()) {
    return nullptr;
  }
  return it->second;
}