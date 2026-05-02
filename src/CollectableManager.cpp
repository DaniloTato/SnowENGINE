#include "CollectableManager.hpp"
#include "GameObject.hpp"

CollectableManager &CollectableManager::getInstance() {
  static CollectableManager instance;
  return instance;
}

void CollectableManager::queueCreateCollectable(const std::string &templateName,
                                                const sf::Vector2f &position) {
  createQueue.push_back({templateName, position});
}

void CollectableManager::queueDeleteCollectable(TangibleObject *obj) {
  deleteQueue.push_back(obj);
}

void CollectableManager::destroyObject(TangibleObject *obj) {
  GameObject::destroy(obj);
}

std::vector<std::string> CollectableManager::getCollectableList() const {
  std::vector<std::string> result;
  result.reserve(templates.size());

  for (const auto &pair : templates)
    result.push_back(pair.first);

  return result;
}