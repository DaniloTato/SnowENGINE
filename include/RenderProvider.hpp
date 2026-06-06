#pragma once

#include "RenderCommand.hpp"

class GameObject;

class RenderProvider {
public:
  virtual ~RenderProvider() = default;

  virtual void appendRenderCommands(std::vector<RenderCommand> &out) const = 0;
};