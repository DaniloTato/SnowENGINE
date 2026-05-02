#include "Commands.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"
#include "RenderableObject.hpp"
#include "RuntimeValue.hpp"
#include "ScriptRunner.hpp"
#include "SnowlangInstance.hpp"
#include "TangibleObject.hpp"

namespace Snowlang::Commands {

namespace {

using ClassCheck = bool (*)(GameObject *);

template <typename T> bool checkType(GameObject *o) { return dynamic_cast<T *>(o) != nullptr; }

const std::unordered_map<std::string_view, ClassCheck> s_ClassChecks = {
    {"game_object", &checkType<GameObject>},
    {"script_runner", &checkType<ScriptRunner>},
    {"tangible_object", &checkType<TangibleObject>},
    {"game_camera", &checkType<GameCamera>},
    {"renderable_object", &checkType<RenderableObject>},
};

} // namespace

RuntimeValue findClassCommand(const CommandContext &ctx) {
  RuntimeValue::List ids;

  size_t max = static_cast<size_t>(ctx.getFlag<float>("max", 10000.f));

  auto it =
      s_ClassChecks.find(SnowlangHelper::RuntimeValueTo<std::string>(ctx.cmd.span)(ctx.args[0]));
  if (it == s_ClassChecks.end()) {
    return ids;
  }

  for (auto *obj : GameObject::getGameObjects()) {
    if (it->second(obj)) {
      ids.emplace_back(static_cast<float>(obj->getId()));
      if (ids.size() >= max) {
        break;
      }
    }
  }

  if (ctx.hasFlag("print")) {
    ctx.snowlang.io.writeLn("Found " + std::to_string(ids.size()) + " objects:\n");
    for (const RuntimeValue &id : ids) {
      ctx.snowlang.io.writeLn("- " + std::to_string(std::get<float>(id.data)) + "\n");
    }
  }

  return ids;
}

} // namespace Snowlang::Commands
