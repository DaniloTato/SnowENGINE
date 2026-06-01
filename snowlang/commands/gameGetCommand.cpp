#include "Commands.hpp"
#include "GameObject.hpp"
#include "RuntimeValue.hpp"
#include "SnowlangInstance.hpp"

namespace Snowlang::Commands {

namespace {

static GameObject *findGameObjectById(float id, Engine &engine) {
  for (const auto &obj : engine.getSceneManager().getCurrentScene()->getObjects()) {
    if (obj->getId() == static_cast<unsigned int>(id)) {
      return obj.get();
    }
  }
  return nullptr;
}

} // namespace

RuntimeValue gameGetCommand(const CommandContext &ctx) {
  if (ctx.args.size() != 1) {
    throwError(SnowErr::Phase::Evaluator, "game_get expects a single integer argument",
               ctx.cmd.span);
  }

  float id = SnowlangHelper::RuntimeValueTo<float>(ctx.cmd.span)(ctx.args[0]);

  GameObject *obj = findGameObjectById(id, *ctx.snowlang.engineRef);
  if (!obj) {
    throwError(SnowErr::Phase::Evaluator,
               "game_get: no GameObject found with id = " + std::to_string(id), ctx.cmd.span);
  }

  GameObjectExposure::Value::Object described = obj->describe();

  return ctx.snowlang.adaptDescriptor(*described);
}

} // namespace Snowlang::Commands
