#include "ParticleRenderComponent.hpp"
#include "Constants.hpp"

void ParticleRenderComponent::appendRenderCommands(
    std::vector<RenderCommand> &out) const {

  for (const auto &p : particles) {

    RenderCommand cmd;

    cmd.position = p.pos;
    cmd.textureRect = p.texRect;
    cmd.color = p.color;
    cmd.texture = p.texture;
    cmd.paralax = p.parallax;
    cmd.layer = Constants::EFFECTS_LAYER;

    out.push_back(cmd);
  }
}