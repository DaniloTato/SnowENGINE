#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "Animation.hpp"
#include "Particle.hpp"
#include "ParticleRenderComponent.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class ParticleManager {
public:
  ~ParticleManager();

  void loadAnimations(Animations &&anim);

  void emitSnow(const sf::Vector2f &pos);
  void emitDust(const sf::Vector2f &pos);
  void emitExplosion(const sf::Vector2f &pos, int count = 12);
  void emitMediumExplosion(const sf::Vector2f &pos, int count = 1,
                           float radius = 1);
  void emitStars(const sf::Vector2f &pos, int count = 1);
  void emitHearts(const sf::Vector2f &pos, int count = 1);
  void emitCross(const sf::Vector2f &pos, int count = 1);
  void emitSmoke(const sf::Vector2f &pos, int count = 1);

  void setWind(const sf::Vector2f &windVec);
  void updateParticles();

  void destroyAll();

  ParticleRenderComponent particleRenderComponent{particles};

private:
  std::vector<Particle> particles;
  sf::Vector2f wind = {0.f, 0.f};

  std::unordered_map<std::string, Animation> cachedAnimations;
};