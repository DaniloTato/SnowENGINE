#include "ParticleManager.hpp"
#include "Animator.hpp"
#include "ColorPalette.hpp"
#include "GameState.hpp"
#include "Helpers.hpp"
#include "PolyRenderizer.hpp"
#include "RenderCommand.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>

ParticleManager::ParticleManager() : GameObject({WindowTypes::MAIN}) {
  persistentAcrossScenes = true;
}

/* HARDCODED PARTICLE ANIMATIONS PATH */

ParticleManager &ParticleManager::getInstance() {
  static ParticleManager instance;
  return instance;
}

void ParticleManager::attachPolyRederizer(PolyRenderizer *polyRenderizer) {
  attachedRenderizer = polyRenderizer;
  Renderizer::registerPair(this, polyRenderizer, false);
}

void ParticleManager::loadAnimations(Animations &&anim) {
  cachedAnimations = std::move(anim);
}

void ParticleManager::update(const GeneralContext &ctx) {
  updateParticles();
  updateRenderCommandBuffer();

  static bool notified = false;

  if (attachedRenderizer) {
    attachedRenderizer->updateRenderCommands(renderCommandBuffer);
  } else {
    if (!notified) {
      std::cout << "[ParticleManager] Warning: No Renderizer Attached.\n";
    }
    notified = true;
  }
}

void ParticleManager::emitSnow(const sf::Vector2f &pos) {
  Particle p;
  p.type = Type::Snow;
  p.pos = pos;

  p.vel = {(float)(rand() % 20 - 10) * 0.1f, 30.f};

  p.gravity = 5.f;

  p.parallax = 1; // may put rand values later, but would have to change the
                  // mothod for snow spawning
  // emit snow(pos, parallax), so when using the ScreenToWorld() function, we
  // can know beforehand the parallax

  p.sinAmplitude = 1.f;
  p.sinFrequency = 5.f;
  p.sinPhase = (float)(rand() % 100) * 0.01f;

  p.shakeIntensity = 0.f;
  p.color = sf::Color(255, 255, 255, 255);

  p.texRect = sf::IntRect(0, 0, 1, 1);

  p.maxLifetime = 7.f;
  p.lifetime = p.maxLifetime;

  particles.push_back(std::move(p));
}

void ParticleManager::emitDust(const sf::Vector2f &pos) {
  Particle p;
  p.type = Type::Dust;
  p.pos = pos;

  p.vel = {(float)(rand() % 50 - 25), (float)(-(rand() % 50))};
  p.gravity = 50.f;

  p.sinAmplitude = 0.f;
  p.sinFrequency = 0.f;
  p.sinPhase = 0.f;

  p.shakeIntensity = 3.f;

  p.color = sf::Color(200, 200, 200, 150);
  p.texRect = sf::IntRect(16, 0, 8, 8);

  p.maxLifetime = 1.5f;
  p.lifetime = p.maxLifetime;

  particles.push_back(std::move(p));
}

void ParticleManager::emitExplosion(const sf::Vector2f &pos, int count) {
  for (int i = 0; i < count; i++) {
    Particle p;
    p.type = Type::Explosion;
    p.pos = pos + sf::Vector2f(Helper::randRange(-16, 16),
                               Helper::randRange(-16, 16));

    const auto &palette = ColorPalette::EXPLOSION_COLORS;
    p.color = *palette[rand() % palette.size()];

    p.animator = std::make_unique<Animator>();
    p.animator->setAnimations(cachedAnimations);
    p.animator->play("small_explosion_once");
    p.texRect = sf::IntRect(8, 8, 8, 8);

    p.maxLifetime = 0.6f;
    p.lifetime = p.maxLifetime;

    particles.push_back(std::move(p));
  }
}

void ParticleManager::emitMediumExplosion(const sf::Vector2f &pos, int count,
                                          float radius) {
  for (int i = 0; i < count; i++) {
    float angle = Helper::randRange(0.f, 2.f * M_PI);
    float r = std::sqrt(Helper::randRange(0.f, 1.f)) * radius;

    sf::Vector2f offset(std::cos(angle) * r, std::sin(angle) * r);

    Particle p;
    p.type = Type::Explosion;
    p.pos = pos + offset;

    p.color = ColorPalette::White;

    p.animator = std::make_unique<Animator>();
    p.animator->setAnimations(cachedAnimations);

    if (rand() % 10 == 0) {
      p.animator->play("medium_explosion_once");
    } else {
      p.animator->play("star_once");
    }

    p.maxLifetime = 2.f;
    p.lifetime = p.maxLifetime;

    particles.push_back(std::move(p));
  }
}

void ParticleManager::emitStars(const sf::Vector2f &pos, int count) {
  for (int i = 0; i < count; i++) {
    Particle p;
    p.type = Type::Stars;
    p.pos = pos;
    p.vel = sf::Vector2f(Helper::randRange(-100, 100),
                         Helper::randRange(-100, 100));

    p.color = sf::Color::White;

    p.animator = std::make_unique<Animator>();
    p.animator->setAnimations(cachedAnimations);
    p.animator->play("star_once");
    p.texRect = sf::IntRect(0, 0, 16, 16);

    p.maxLifetime = 1.f;
    p.lifetime = p.maxLifetime;

    particles.push_back(std::move(p));
  }
}

void ParticleManager::emitHearts(const sf::Vector2f &pos, int count) {
  for (int i = 0; i < count; i++) {
    Particle p;
    p.type = Type::Heart;
    p.pos = pos;
    p.vel = sf::Vector2f(Helper::randRange(-100, 100),
                         Helper::randRange(-100, 100));

    p.color = sf::Color::White;

    p.animator = std::make_unique<Animator>();
    p.animator->setAnimations(cachedAnimations);
    p.animator->play("heart_once");
    p.texRect = sf::IntRect(0, 0, 16, 16);

    p.maxLifetime = 3.f;
    p.lifetime = p.maxLifetime;

    particles.push_back(std::move(p));
  }
}

void ParticleManager::emitCross(const sf::Vector2f &pos, int count) {
  for (int i = 0; i < count; i++) {
    Particle p;
    p.type = Type::Cross;
    p.pos = pos;
    p.vel = sf::Vector2f(Helper::randRange(-100, 100),
                         Helper::randRange(-100, 100));

    p.color = sf::Color::White;

    p.animator = std::make_unique<Animator>();
    p.animator->setAnimations(cachedAnimations);
    p.animator->play("cross_once");
    p.texRect = sf::IntRect(0, 0, 16, 16);

    p.maxLifetime = 1.f;
    p.lifetime = p.maxLifetime;

    particles.push_back(std::move(p));
  }
}

void ParticleManager::emitSmoke(const sf::Vector2f &pos, int count) {
  for (int i = 0; i < count; i++) {
    Particle p;
    p.type = Type::Smoke;
    p.pos = pos;

    p.vel = {Helper::randRange(-10.f, 10.f), Helper::randRange(-20.f, -40.f)};

    p.gravity = -5.f;

    p.sinAmplitude = Helper::randRange(0.5f, 1.5f);
    p.sinFrequency = Helper::randRange(1.f, 3.f);
    p.sinPhase = Helper::randRange(0.f, 3.f);

    p.texRect = sf::IntRect(16, 0, 12, 12);

    if (rand() % 2 == 0) {
      p.color = ColorPalette::SoftViolet;
    } else {
      p.color = ColorPalette::VividIndigo;
    }
    p.color.a = 200;

    p.maxLifetime = Helper::randRange(1.5f, 3.f);
    p.lifetime = p.maxLifetime;

    particles.push_back(std::move(p));
  }
}

void ParticleManager::setWind(const sf::Vector2f &windVec) { wind = windVec; }

void ParticleManager::updateParticles() {

  float dt = GameState::getInstance().dt();

  for (auto &p : particles) {

    if (p.animator) {
      p.animator->update();
      p.texRect = p.animator->getCurrentFrame();

      if (p.animator->animationFinished()) {
        p.forceDeath = true;
      }
    }

    p.lifetime -= dt;
    if (p.lifetime <= 0.f)
      continue;

    p.vel += wind * dt;
    p.vel.y += p.gravity * dt;
    p.pos += p.vel * dt;

    if (p.type == Type::Snow || p.type == Type::Smoke) {
      float offset =
          std::sin((p.maxLifetime - p.lifetime) * p.sinFrequency + p.sinPhase) *
          p.sinAmplitude;
      p.pos.x += offset * dt * 60.f;
    }

    if (p.type == Type::Explosion || p.type == Type::Smoke) {
      float alpha = p.lifetime / p.maxLifetime;
      p.color.a = static_cast<sf::Uint8>(255 * alpha);
    }
  }

  std::erase_if(particles, [](const Particle &p) {
    return p.lifetime <= 0.f || p.forceDeath;
  });
}

void ParticleManager::updateRenderCommandBuffer() {

  renderCommandBuffer.clear();

  for (auto &p : particles) {

    RenderCommand cmd;
    cmd.rect = p.texRect;
    cmd.pos = p.pos;
    cmd.color = p.color;
    cmd.overrideParalax = p.parallax;

    if (p.shakeIntensity > 0.f) {
      float t = (p.maxLifetime - p.lifetime) * 30.f;
      cmd.pos.x += std::sin(t * 2.7f) * p.shakeIntensity;
      cmd.pos.y += std::cos(t * 3.7f) * p.shakeIntensity;
    }

    renderCommandBuffer.push_back(cmd);
  }
}

void ParticleManager::destroyAll() { particles.clear(); }

void ParticleManager::onSceneUnload() {
  destroyAll();

  renderCommandBuffer.clear();

  delete attachedRenderizer;
  attachedRenderizer = nullptr;

  wind = {0.f, 0.f};
}