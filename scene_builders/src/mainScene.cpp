#include "mainScene.hpp"

#include "GameState.hpp"
#include "Helpers.hpp"
#include "ParticleManager.hpp"
#include "PolyRenderizer.hpp"
#include "RenderableObject.hpp"

#include "RegistryMacros.hpp"
#include "SceneBuilderRegistry.hpp" // IWYU pragma: keep

namespace SceneBuilder {
void mainScene() {

  GameState::getInstance().createCamera(CameraTypes::MAIN, {WindowTypes::MAIN});

  sf::Texture particleTexture;
  particleTexture.loadFromFile(
      Helper::getPath("assets/sprites/misc/particle.png"));

  RenderizerParameters polyRenderizerParams{
      .window = *GameState::getInstance().getMainWindow(),
      .texture = particleTexture,
      .camera = GameState::getInstance().getMainCamera()};

  auto *pr = new PolyRenderizer(polyRenderizerParams);
  ParticleManager::getInstance().attachPolyRederizer(pr);

  sf::Texture toddTexture;
  toddTexture.loadFromFile(Helper::getPath("assets/sprites/misc/todd.png"));

  RenderizerParameters params = {
      .window = *GameState::getInstance().getMainWindow(),
      .texture = toddTexture,
      .camera = GameState::getInstance().getMainCamera()};

  auto *image = new RenderableObject(params);

  image->position = {100.f, 100.f};

  /* remember to build your context

    GeneralContext ctx = {.playerPosition = &(player->position),
                          .textParams = dialogueManager.getAttachedTextParams(),
                          .player = player,
                          .bulletTexture = &bulletTexture};
    GameState::getInstance().updateGeneralContext(ctx);

  */
}
} // namespace SceneBuilder

REGISTER_SCENE_BUILDER("mainScene", SceneBuilder::mainScene);
