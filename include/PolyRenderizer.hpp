#pragma once
#include "RenderCommand.hpp"
#include "Renderizer.hpp"
#include <vector>

class PolyRenderizer : public Renderizer {
public:
  PolyRenderizer(const RenderizerParameters &params);

  void updateRenderCommands(std::vector<RenderCommand> &renderCommands);
  void render(GameObject *obj) override;

private:
  std::vector<RenderCommand> renderCommands;
};