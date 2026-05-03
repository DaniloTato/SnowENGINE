#pragma once

#include <SFML/Graphics.hpp>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "GameState.hpp"
#include <filesystem>

namespace Helper {

struct TriggerOnce {
  bool fired = false;

  template <typename Fn> void check(bool condition, Fn &&fn) {
    if (condition && !fired) {
      fired = true;
      fn();
    }
  }
};

class DoEvery {
private:
  float counter = 0;

public:
  template <typename Fn> void operator()(float seconds, Fn &&fn) {
    counter += GameState::getInstance().dt();
    if (counter >= seconds) {
      counter = 0;
      fn();
    }
  }
};

template <typename T>
constexpr T lerp(const T &actual, const T &desired, float speed) {
  return actual + (desired - actual) * speed;
}

sf::Texture &loadTexture(const std::string &path);
float distance(const sf::Vector2f &pos1, const sf::Vector2f &pos2);
sf::FloatRect makeRectFromPoints(float x1, float y1, float x2, float y2);
std::filesystem::path getExecutableDir();
std::string getPath(const std::string &relativePath);
float randRange(float min, float max);

} // namespace Helper