#pragma once
#include "Animator.hpp"
#include <SFML/Graphics.hpp>
#include <cstdint>

struct Particle {

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

  sf::Texture *texture = nullptr;

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