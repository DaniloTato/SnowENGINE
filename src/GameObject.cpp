#include "GameObject.hpp"
#include "GameObjectExposure.hpp"

unsigned int GameObject::nextId = 0;

bool GameObject::UpdateDomain::matches(WindowManager &wm, WindowID id) const {

  if (std::ranges::find(windows, id) != windows.end()) {
    return true;
  }

  auto domain = wm.getDomainOfWindow(id);

  if (std::ranges::find(domains, domain) != domains.end()) {
    return true;
  }

  return false;
}

GameObject::GameObject(UpdateDomain updateDomain, sf::Vector2f pos)
    : position(pos), id(nextId++), updateDomain(std::move(updateDomain)) {}

GameObject::ID GameObject::getId() { return id; }

GameObjectExposure::Value::Object GameObject::describe() {

  auto desc = std::make_shared<GameObjectExposure::Descriptor>();

  desc->fields["pos"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [this] {
            return GameObjectExposure::Descriptor::describeVector2f(position);
          });

  desc->fields["id"] = GameObjectExposure::makePublicField<u_int32_t>(id.val);

  desc->fields["offset"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [this] {
            return GameObjectExposure::Descriptor::describeVector2f(offset);
          });

  return desc;
}

bool GameObject::isUpdateDomainPaused(WindowManager &wm) {

  auto isAnyWindowActive = [&](auto &&ids) {
    for (auto id : ids) {
      if (!wm.isWindowPaused(id))
        return true;
    }

    return false;
  };

  if (isAnyWindowActive(updateDomain.windows)) {
    return false;
  }

  for (auto domain : updateDomain.domains) {
    if (isAnyWindowActive(wm.getByDomain(domain))) {
      return false;
    }
  }

  return true;
}

void GameObject::destroyLater() { pendingDestroy = true; }

bool GameObject::isPendingDestroy() const { return pendingDestroy; }

void GameObject::setUpdateDomain(const UpdateDomain &newDomain) {
  updateDomain = newDomain;
}