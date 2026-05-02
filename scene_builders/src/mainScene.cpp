#include "mainScene.hpp"

#include "AnimationFactory.hpp"
#include "GameState.hpp"
#include "ScriptRunner.hpp"
#include "TangibleObject.hpp"
#include "TextureManager.hpp"

#include "ObjectBuilder.hpp"

/*Scripts*/
#include "basicMovementScript.hpp"
#include "terminalCreationScript.hpp"

#include "RegistryMacros.hpp"
#include "SceneBuilderRegistry.hpp" // IWYU pragma: keep

namespace SceneBuilder {
void mainScene() {

  GameState::getInstance().createCamera(CameraTypes::MAIN, {WindowTypes::MAIN});

  /* Setup in case we'd like to have particles in the scene.

  RenderizerParameters polyRenderizerParams{
      .window = GameState::getInstance().getMainWindow(),
      .texture = &TextureManager::getInstance().get("particles"),
      .camera = GameState::getInstance().getMainCamera()};

  auto *pr = new PolyRenderizer(polyRenderizerParams);
  ParticleManager::getInstance().attachPolyRederizer(pr);
  ParticleManager::getInstance().loadAnimations(
      Animator::getAsepriteJSONAnimations(
          Helper::getPath("assets/animations/particles.json")));
  */

  auto *image = ObjectBuilder<TangibleObject>("todd")
                    .at(100, 100)
                    .withEmptyAnimation(16, 16)
                    .build();

  //   RenderizerParameters params = {
  //       .window = GameState::getInstance().getMainWindow(),
  //       .texture = &TextureManager::getInstance().get("todd"),
  //       .camera = GameState::getInstance().getMainCamera()};

  //   auto *image = new TangibleObject(params, AnimationFactory::empty(16,
  //   16)); image->position = {100.f, 100.f}; image->animator.play("idle");

  image->scripter.addScript("simpleMovement", Scripts::basicMovementScript);

  ScriptRunner *sr =
      new ScriptRunner({WindowTypes::MAIN, WindowTypes::TERMINAL});
  sr->scripter.addScript("terminalCreation", Scripts::terminalCreationScript);

  GeneralContext ctx = {.player = nullptr};
  GameState::getInstance().updateGeneralContext(ctx);
}
} // namespace SceneBuilder

REGISTER_SCENE_BUILDER("mainScene", SceneBuilder::mainScene);
