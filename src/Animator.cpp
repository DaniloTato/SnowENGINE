#include "Animator.hpp"
#include "GameState.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
struct RawFrame {
  int index;
  sf::IntRect rect;
  float duration;
};
} // namespace

void Animator::loadAsepriteAnimations(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "[Animator] ERROR: Cannot open " << filename << "\n";
    return;
  }

  json j;
  file >> j;

  animations.clear();

  struct ParsedFrame {
    int index;
    sf::IntRect rect;
    float duration;
  };

  std::unordered_map<std::string, std::vector<ParsedFrame>> grouped;

  for (auto &[key, frameData] : j["frames"].items()) {

    size_t hash = key.find('#');
    size_t space = key.find(' ', hash);
    size_t dot = key.rfind('.');

    if (hash == std::string::npos || space == std::string::npos)
      continue;

    std::string animName = key.substr(hash + 1, space - hash - 1);
    int frameIndex = std::stoi(key.substr(space + 1, dot - space - 1));

    auto f = frameData["frame"];

    grouped[animName].push_back({frameIndex,
                                 sf::IntRect(f["x"], f["y"], f["w"], f["h"]),
                                 frameData["duration"].get<float>() / 1000.f});
  }

  // Build animations
  for (auto &[name, frames] : grouped) {
    Animation anim;
    anim.loop = name.find("_once") == std::string::npos;

    // Sort by local frame index
    std::ranges::sort(frames, std::ranges::less{}, &ParsedFrame::index);

    int expectedRow = -1;
    bool rowMismatch = false;

    for (auto &pf : frames) {
      anim.frames.push_back({pf.rect, pf.duration});

      if (expectedRow == -1)
        expectedRow = pf.rect.top;
      else if (pf.rect.top != expectedRow)
        rowMismatch = true;
    }

    if (rowMismatch)
      std::cout
          << "[Animator] WARNING: Animation spans multiple sprite rows!\n";

    animations[name] = anim;
  }
}

Animations Animator::getAsepriteJSONAnimations(const std::string &filename) {
  static Animator tempAnimator;
  tempAnimator.loadAsepriteAnimations(filename);
  return tempAnimator.animations;
}

void Animator::addAnimation(const std::string &name, const Animation &anim) {
  animations[name] = anim;
}

void Animator::setAnimations(Animations &newAnimations) {
  animations = newAnimations;
}

void Animator::play(const std::string &name) {
  if (currentState == name)
    return;

  auto it = animations.find(name);
  if (it == animations.end()) {
    std::cout << "[ANIMATOR] WARNING. Animation name not found: " << name
              << "\n";
    return;
  }

  currentAnim = &it->second;
  currentState = name;
  index = 0;
  timer = 0.f;
  finished = false;
}

void Animator::update() {
  if (!currentAnim || finished)
    return;

  if (currentAnim->frames.empty())
    return;

  timer += speedMultiplier * GameState::getInstance().dt();

  const Animation::Frame &frame = currentAnim->frames[index];

  if (timer >= frame.duration) {
    timer -= frame.duration;
    index++;

    if (index >= currentAnim->frames.size()) {
      if (currentAnim->loop) {
        index = 0;
      } else {
        index = currentAnim->frames.size() - 1;
        finished = true;
      }
    }
  }
}

const sf::IntRect &Animator::getCurrentFrame() const {
  static sf::IntRect dummy{0, 0, 0, 0};

  if (!currentAnim || currentAnim->frames.empty())
    return dummy;

  return currentAnim->frames[index].rect;
}

void Animator::setSpeedMultiplier(float multiplier) {
  speedMultiplier = multiplier;
}

bool Animator::animationFinished() const { return finished; }