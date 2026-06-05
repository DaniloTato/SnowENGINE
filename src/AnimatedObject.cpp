#include "AnimatedObject.hpp"
#include "GeneralContext.hpp"
#include "SpriteComponent.hpp"

AnimatedObject::AnimatedObject(RenderizerParameters params)
    : GameObject(UpdateDomain(WindowManager::Set::MAIN)) {}

void AnimatedObject::update(const GeneralContext &ctx) {
  scripter.runScripts(*this, ctx);
  animator.update();

  if (spriteComponent) {
    spriteComponent->setRect(animator.getCurrentFrame(), 1);
  }
}