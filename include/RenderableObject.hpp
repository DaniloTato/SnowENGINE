#pragma once

#include "GameObject.hpp"
#include "RenderizerParameters.hpp"
#include "Scripter.hpp"

class RenderableObject : public GameObject {
public:
  RenderableObject(const RenderizerParameters &params);
  void update(const GeneralContext &ctx) override;
  Scripter<RenderableObject> scripter;
};