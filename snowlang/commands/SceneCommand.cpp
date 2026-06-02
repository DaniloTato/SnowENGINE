#include "Commands.hpp"
#include "RuntimeValue.hpp"
#include "SceneBuilderRegistry.hpp"
#include "SceneManager.hpp"
#include "SnowlangHelper.hpp"
#include "SnowlangInstance.hpp"

namespace Snowlang::Commands {

RuntimeValue sceneCommand(const CommandContext &ctx) {

  // auto &sceneManager = ctx.snowlang.engineRef->getSceneManager();

  // --reload
  if (ctx.hasFlag("reload")) {
    // sceneManager.reloadCurrentScene(*ctx.snowlang.engineRef);
    return {true};
  }

  if (ctx.hasFlag("print")) {
    auto filter = ctx.getFlag<std::string>("print", "");

    // tight coupling
    const std::unordered_map<std::string, SceneManager::SceneFactory> &scenes =
        SceneBuilderRegistry::getAllItems();

    for (const auto &[name, func] : scenes) {
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

  bool success = true;
  // bool success = sceneManager.loadScene(sceneName, *ctx.snowlang.engineRef);

  if (!success) {
    throwError(SnowErr::Phase::Evaluator,
               "[scene] Could not load the following scene: " + sceneName, ctx.cmd.span);
  }

  return {success};
}

} // namespace Snowlang::Commands