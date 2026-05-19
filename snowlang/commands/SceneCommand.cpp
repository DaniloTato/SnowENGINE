#include "Commands.hpp"
#include "RuntimeValue.hpp"
#include "SceneManager.hpp"
#include "SnowlangHelper.hpp"
#include "SnowlangInstance.hpp"

namespace Snowlang::Commands {

RuntimeValue sceneCommand(const CommandContext &ctx) {

  auto &sceneManager = SceneManager::getInstance();

  // --reload
  if (ctx.hasFlag("reload")) {
    sceneManager.reloadCurrentScene(*ctx.snowlang.windowManagerRef);
    return {true};
  }

  if (ctx.hasFlag("print")) {
    auto filter = ctx.getFlag<std::string>("print", "");

    const auto scenes = sceneManager.getRegisteredScenes();

    for (const auto &name : scenes) {
      if (filter.empty() || name.find(filter) != std::string::npos) {
        ctx.snowlang.io.writeLn("\"" + name + "\"");
      }
    }

    return {true};
  }

  if (ctx.args.size() != 1) {
    throwError(SnowErr::Phase::Evaluator, "[scene] Expected exactly 1 string argument",
               ctx.cmd.span);
  }

  const auto &sceneName = SnowlangHelper::RuntimeValueTo<std::string>(ctx.cmd.span)(ctx.args[0]);

  bool success = sceneManager.loadScene(sceneName, *ctx.snowlang.windowManagerRef);

  if (!success) {
    throwError(SnowErr::Phase::Evaluator,
               "[scene] Could not load the following scene: " + sceneName, ctx.cmd.span);
  }

  return {success};
}

} // namespace Snowlang::Commands