#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>

struct Animation {
  struct Frame {
    sf::IntRect rect;
    float duration;
  };

  std::string name;
  std::vector<Frame> frames;
  bool loop = true;
};

using Animations = std::unordered_map<std::string, struct Animation>;