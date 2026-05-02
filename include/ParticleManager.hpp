#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "Animation.hpp"
#include "Animator.hpp"
#include "GameObject.hpp"
#include "PolyRenderizer.hpp"
#include "RenderCommand.hpp"
#include "SceneAware.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class ParticleManager : public GameObject, public SceneAware {
public:
  enum class Type : std::uint8_t {
    Snow,
    Dust,
    Explosion,
    Stars,
    Smoke,
    ExplosionMedium,
    Cross,
    Heart
  };

  struct Particle {
    Type type;
    sf::Vector2f pos;
    sf::Vector2f vel;
    float lifetime;
    float maxLifetime;
    float gravity;

    float sinAmplitude;
    float sinFrequency;
    float sinPhase;

    float shakeIntensity;

    float parallax = 1.f;

    bool forceDeath = false;

    sf::Color color;
    sf::IntRect texRect;
    std::unique_ptr<Animator> animator;
  };

  static ParticleManager &getInstance();

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
  void updateRenderCommandBuffer();

  void update(const GeneralContext &ctx) override;

  void attachPolyRederizer(PolyRenderizer *polyRenderizer);

  void destroyAll();

  void onSceneUnload() override;

private:
  ParticleManager();

  std::vector<Particle> particles;
  PolyRenderizer *attachedRenderizer;
  sf::Vector2f wind = {0.f, 0.f};

  std::unordered_map<std::string, Animation> cachedAnimations;

  std::vector<RenderCommand> renderCommandBuffer;
};