#include "AnimationFactory.hpp"

Animations AnimationFactory::empty(int w, int h) {
  Animation idle;
  idle.name = "idle";
  idle.frames = {{.rect = {0, 0, w, h}, .duration = 1.f}};

  return {{"idle", idle}};
}