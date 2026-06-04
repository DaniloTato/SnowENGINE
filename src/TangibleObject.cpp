#include "TangibleObject.hpp"
#include "GeneralContext.hpp"
#include "SoundManager.hpp"
#include "SpriteComponent.hpp"
#include <SFML/Audio.hpp>

TangibleObject::TangibleObject(RenderizerParameters params,
                               const Animations &cachedAnimations)
    : GameObject(UpdateDomain(params.window)) {
  animator.setAnimations(cachedAnimations);
}

void TangibleObject::update(const GeneralContext &ctx) {
  collider.computeCollisionGrid(position);
  scripter.runScripts(*this, ctx);
  animator.update();

  if (spriteComponent) {
    spriteComponent->setRect(animator.getCurrentFrame(), direction);
  }
}

void TangibleObject::playSound(const std::string &id, float volume) {
  sf::Sound &s = sounds[id];
  s.setBuffer(SoundManager::getInstance().get(id));
  s.setVolume(volume);
  s.play();
}

bool TangibleObject::isPlayingAnySound() const {
  for (const auto &[id, sound] : sounds) {
    if (sound.getStatus() == sf::Sound::Playing && sound.getVolume() > 0.f) {
      return true;
    }
  }
  return false;
}

GameObjectExposure::Value::Object TangibleObject::describe() {
  auto desc = std::make_shared<GameObjectExposure::Descriptor>();

  desc->fields["scripts"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [this] { return scripter.describe(); });

  desc->fields["physics"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [this] { return physics.describe(); });

  auto gameObjectDescriptions = GameObject::describe();
  desc->fields.merge(gameObjectDescriptions->fields);

  return desc;
}