#pragma once

#include "Animator.hpp"
#include "GameObject.hpp"
#include "RenderizerParameters.hpp"
#include "Scripter.hpp"

class AnimatedObject : public GameObject {
public:
  AnimatedObject(RenderizerParameters params);
  void update(const GeneralContext &ctx) override;
  Scripter<AnimatedObject> scripter;
  Animator animator;
};