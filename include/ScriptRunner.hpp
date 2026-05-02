#pragma once

#include "GameObject.hpp"
#include "Scripter.hpp"

class ScriptRunner : public GameObject {
public:
  ScriptRunner(GameObject::UpdateDomain updateDomain);
  void update(const GeneralContext &ctx) override;
  GameObjectExposure::Value::Object describe() override;
  Scripter<ScriptRunner> scripter;
};