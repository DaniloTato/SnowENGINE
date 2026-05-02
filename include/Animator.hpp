#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

#include "Animation.hpp"

class Animator {
public:
  Animator() = default;

  void addAnimation(const std::string &name, const Animation &anim);
  void setAnimations(Animations &newAnimations);
  void update();

  void play(const std::string &name);

  void setSpeedMultiplier(float multiplier);

  void loadAsepriteAnimations(const std::string &filename);
  static Animations getAsepriteJSONAnimations(const std::string &filename);

  [[nodiscard]] const sf::IntRect &getCurrentFrame() const;

  [[nodiscard]] bool animationFinished() const;

private:
  Animations animations;

  const Animation *currentAnim = nullptr;
  std::string currentState;

  float timer = 0.f;
  std::optional<std::string> current;
  size_t index = 0;

  float speedMultiplier = 1.f;
  bool finished = false;
};