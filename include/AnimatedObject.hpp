#pragma once

#include "Animator.hpp"
#include "Renderizer.hpp"

class AnimatedObject : public GameObject {
public:
  AnimatedObject(RenderizerParameters params);
  void update(const GeneralContext &ctx) override;
  Renderizer renderizer;
  Scripter<AnimatedObject> scripter;
  Animator animator;
};