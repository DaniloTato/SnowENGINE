#include "Commands.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"
#include "RuntimeValue.hpp"
#include "Scripter.hpp"
#include "SnowlangInstance.hpp"
#include "TangibleObject.hpp"

namespace Snowlang::Commands {

namespace {

/*VERY EVIDENT CASE OF TIGHT COUPLING*/
/*CONNECT DIRECTLY TO REGISTRY VIA IO*/

std::unordered_map<std::string, Scripter<GameCamera>::ScriptFunc> cameraFunctionMap = {

};

std::unordered_map<std::string, Scripter<TangibleObject>::ScriptFunc> tangibleFunctionMap = {

};

} // namespace

RuntimeValue addScriptCommand(const CommandContext &ctx) {

  if (ctx.args.size() != 3) {
    throwError(SnowErr::Phase::Evaluator, "[find_class] expects 3 arg", ctx.cmd.span);
  }

  auto id = SnowlangHelper::RuntimeValueTo<float>(ctx.cmd.span)(ctx.args[0]);
  GameObject *object =
      ctx.snowlang.engineRef->getSceneManager().getCurrentScene()->findObjectById((size_t)id);

  const auto &objectType = SnowlangHelper::RuntimeValueTo<std::string>(ctx.cmd.span)(ctx.args[1]);
  const auto &scriptName = SnowlangHelper::RuntimeValueTo<std::string>(ctx.cmd.span)(ctx.args[2]);

  if (objectType == "tangible") {
    if (auto ptr = dynamic_cast<TangibleObject *>(object)) {
      auto it = tangibleFunctionMap.find(scriptName);

      if (it == tangibleFunctionMap.end()) {
        throwError(SnowErr::Phase::Evaluator,
                   "[find_class] " + scriptName + " not found on registry.", ctx.cmd.span);
      }

      // TIGHT COUPLING
      Scripter<TangibleObject>::ScriptFunc fn = it->second;

      ptr->scripter.addScript(fn);
    } else {
      throwError(SnowErr::Phase::Evaluator,
                 "[find_class] passed along an id belonging to a non-" + objectType + " class.",
                 ctx.cmd.span);
    }

  } else if (objectType == "camera") {
    if (auto ptr = dynamic_cast<GameCamera *>(object)) {
      auto it = cameraFunctionMap.find(scriptName);

      if (it == cameraFunctionMap.end()) {
        throwError(SnowErr::Phase::Evaluator,
                   "[find_class] " + scriptName + " not found on registry.", ctx.cmd.span);
      }

      Scripter<GameCamera>::ScriptFunc fn = it->second;

      ptr->scripter.addScript(fn);
    } else {
      throwError(SnowErr::Phase::Evaluator,
                 "[find_class] passed along an id belonging to a non-" + objectType + " class.",
                 ctx.cmd.span);
    }

  } else {
    throwError(SnowErr::Phase::Evaluator, "[find_class] unknown object type.", ctx.cmd.span);
  }

  return {true};
}

} // namespace Snowlang::Commands
