#include "GameObject.hpp"
#include "GameObjectExposure.hpp"
#include "GameState.hpp"
#include "SFML/System/Vector2.hpp"
#include <vector>

unsigned int GameObject::nextId = 0;

GameObject::GameObject(UpdateDomain updateDomain, sf::Vector2f pos)
    : position(pos), updateDomain(std::move(updateDomain)) {
  s_gameObjects.push_back(this);
  id = nextId++;
}

GameObject::~GameObject() {
  auto it = std::ranges::find(s_gameObjects, this);
  if (it != s_gameObjects.end()) {
    s_gameObjects.erase(it);
  }
}

void GameObject::destroy(GameObject *g) {
  std::vector<GameObject *> &list = GameObject::s_gameObjects;

  // Interesting use of if statement with an initizalizer. C++17

  /*auto it = std::ranges::find(list, g); — This runs first. It creates the
  variable it.

  Second ExprStmt is the boolean that determines if the body of the if is
  executed. */

  if (auto it = std::ranges::find(list, g); it != list.end()) {
    *it = list.back();
    list.pop_back();
  }

  delete g;
}

std::vector<GameObject *> GameObject::s_gameObjects;

std::vector<GameObject *> &GameObject::getGameObjects() {
  return s_gameObjects;
}

void GameObject::destroySceneObjects() {
  auto &objects = getGameObjects();

  for (int i = static_cast<int>(objects.size()) - 1; i >= 0; --i) {
    GameObject *obj = objects[i];

    if (!obj->isPersistentAcrossScenes()) {
      destroy(obj);
    }
  }
}

void GameObject::makePersistentAcrossScenes() { persistentAcrossScenes = true; }

bool GameObject::isPersistentAcrossScenes() const {
  return persistentAcrossScenes;
}

unsigned int GameObject::getId() { return id; }

void getTerminalObject() {}

GameObjectExposure::Value::Object GameObject::describe() {

  auto desc = std::make_shared<GameObjectExposure::Descriptor>();

  desc->fields["pos"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [this] {
            return GameObjectExposure::Descriptor::describeVector2f(position);
          });

  desc->fields["id"] = GameObjectExposure::makePublicField<unsigned int>(id);

  desc->fields["offset"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [this] {
            return GameObjectExposure::Descriptor::describeVector2f(offset);
          });

  return desc;
}

bool GameObject::isUpdateDomainPaused() {
  for (const auto &i : updateDomain) {
    if (!GameState::getInstance().getWindowByType(i).isPaused()) {
      return false;
    }
  }

  return true;
}

GameObject::UpdateDomain GameObject::UniversalDomain() {
  UpdateDomain domain;
  for (size_t i = 0; i < static_cast<size_t>(WindowTypes::COUNT); ++i)
    domain.push_back(static_cast<WindowTypes>(i));
  return domain;
}

GameObject *GameObject::findGameObjectById(float id) {
  for (auto *obj : GameObject::getGameObjects()) {
    if (obj->getId() == static_cast<unsigned int>(id))
      return obj;
  }
  return nullptr;
}