#pragma once

#include "GameObject.hpp"
#include "Renderizer.hpp"
#include "Scripter.hpp"

class RenderableObject : public GameObject {
public:
  RenderableObject(const RenderizerParameters &params);
  void update(const GeneralContext &ctx) override;
  Renderizer renderizer;
  Scripter<RenderableObject> scripter;
};