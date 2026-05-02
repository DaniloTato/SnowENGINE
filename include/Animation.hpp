#pragma once
#include <SFML/Graphics.hpp>

struct Animation {
  struct Frame {
    sf::IntRect rect;
    float duration;
  };

  std::string name;
  std::vector<Frame> frames;
  bool loop = true;
};