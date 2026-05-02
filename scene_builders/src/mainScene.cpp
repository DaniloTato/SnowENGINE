#include "mainScene.hpp"

#include "GameState.hpp"
#include "ParticleManager.hpp"
#include "PolyRenderizer.hpp"
#include "RenderableObject.hpp"
#include "ScriptRunner.hpp"
#include "TextureManager.hpp"

/*Scripts*/
#include "terminalCreationScript.hpp"

#include "RegistryMacros.hpp"
#include "SceneBuilderRegistry.hpp" // IWYU pragma: keep

namespace SceneBuilder {
void mainScene() {

  GameState::getInstance().createCamera(CameraTypes::MAIN, {WindowTypes::MAIN});

  sf::Texture dummyTexture;

  RenderizerParameters polyRenderizerParams{
      .window = GameState::getInstance().getMainWindow(),
      .texture = &dummyTexture,
      .camera = GameState::getInstance().getMainCamera()};

  auto *pr = new PolyRenderizer(polyRenderizerParams);
  ParticleManager::getInstance().attachPolyRederizer(pr);

  RenderizerParameters params = {
      .window = GameState::getInstance().getMainWindow(),
      .texture = &TextureManager::getInstance().get("todd"),
      .camera = GameState::getInstance().getMainCamera()};

  auto *image = new RenderableObject(params);
  image->position = {100.f, 100.f};

  ScriptRunner *sr =
      new ScriptRunner({WindowTypes::MAIN, WindowTypes::TERMINAL});
  sr->scripter.addScript("terminalCreation", Scripts::terminalCreationScript);

  GeneralContext ctx = {.player = nullptr};
  GameState::getInstance().updateGeneralContext(ctx);
}
} // namespace SceneBuilder

REGISTER_SCENE_BUILDER("mainScene", SceneBuilder::mainScene);
