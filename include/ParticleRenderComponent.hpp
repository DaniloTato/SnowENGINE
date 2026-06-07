#pragma once

#include "Particle.hpp"
#include "RenderProvider.hpp"

class ParticleManager;

class ParticleRenderComponent : public RenderProvider {
private:
  std::vector<Particle> &particles;

public:
  ParticleRenderComponent(std::vector<Particle> &particlesRef)
      : particles(particlesRef) {}
  void appendRenderCommands(std::vector<RenderCommand> &out) const override;
};