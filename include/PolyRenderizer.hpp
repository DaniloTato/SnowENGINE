#pragma once
#include "OldRenderCommand.hpp"
#include "Renderizer.hpp"
#include <vector>

class PolyRenderizer : public Renderizer {
public:
  PolyRenderizer(const RenderizerParameters &params);

  void updateRenderCommands(std::vector<OldRenderCommand> &renderCommands);
  void render(GameObject *obj) override;

private:
  std::vector<OldRenderCommand> renderCommands;
};