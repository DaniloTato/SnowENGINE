#include "TangibleObject.hpp"
#include "GeneralContext.hpp"
#include "SoundManager.hpp"
#include <SFML/Audio.hpp>

#include "GameState.hpp"

TangibleObject::TangibleObject(RenderizerParameters params,
                               Animations cachedAnimations)
    : GameObject({GameState::getInstance().windowPtrToType(&params.window)}),
      renderizer(params) {
  animator.setAnimations(cachedAnimations);
  Renderizer::registerPair(this, &renderizer, params.registerAsRectShape);
}

void TangibleObject::update(const GeneralContext &ctx) {
  collider.computeCollisionGrid(position);
  scripter.runScripts(*this, ctx);
  animator.update();
  renderizer.setRect(animator.getCurrentFrame(), direction);
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