#include "ScriptRunner.hpp"
#include "Scripter.hpp"

ScriptRunner::ScriptRunner(GameObject::UpdateDomain updateDomain)
    : GameObject(std::move(updateDomain)) {}

void ScriptRunner::update(const GeneralContext &ctx) {
  scripter.runScripts(*this, ctx);
}

GameObjectExposure::Value::Object ScriptRunner::describe() {
  auto desc = std::make_shared<GameObjectExposure::Descriptor>();

  desc->fields["scripts"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [this] { return scripter.describe(); });

  auto gameObjectDescriptions = GameObject::describe();
  desc->fields.merge(gameObjectDescriptions->fields);

  return desc;
}