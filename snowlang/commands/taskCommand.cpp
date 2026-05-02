#include "CommandSignature.hpp"
#include "SnowErr.hpp"
#include "SnowlangHelper.hpp"
#include "SnowlangInstance.hpp"

namespace Snowlang::Commands {

RuntimeValue taskCommand(const CommandContext &ctx) {

  if (ctx.hasFlag("list")) {
    const auto &tasks = ctx.snowlang.getTasks();

    if (tasks.empty()) {
      ctx.snowlang.io.writeLn("No scheduled tasks.");
      return {true};
    }

    ctx.snowlang.io.writeLn("Scheduled tasks:");
    for (const auto &task : tasks) {
      ctx.snowlang.io.writeLn(task.describe());
    }

    return {true};
  }

  if (ctx.hasFlag("killall")) {
    ctx.snowlang.killAllTasks();
    return {true};
  }

  if (ctx.hasFlag("kill")) {
    float id = SnowlangHelper::RuntimeValueTo<float>(ctx.cmd.span)(ctx.getFlag("kill", 0.f));

    bool ok = ctx.snowlang.killTask(static_cast<size_t>(id));
    return {ok};
  }

  if (ctx.hasFlag("pause")) {
    float id = SnowlangHelper::RuntimeValueTo<float>(ctx.cmd.span)(ctx.getFlag("pause", 0.f));

    bool ok = ctx.snowlang.togglePauseTask(static_cast<size_t>(id));
    return {ok};
  }

  if (!ctx.hasFlag("do") || !ctx.hasFlag("every")) {
    throwError(SnowErr::Phase::Evaluator,
               "task requires --do and --every, or --kill / --killall / --pause", ctx.cmd.span);
  }

  RuntimeValue::Lambda lambda = SnowlangHelper::RuntimeValueTo<RuntimeValue::Lambda>(ctx.cmd.span)(
      ctx.getFlag("do", RuntimeValue::Lambda{}));

  double interval = SnowlangHelper::RuntimeValueTo<float>(ctx.cmd.span)(ctx.getFlag("every", 1.f));

  if (interval <= 0) {
    throwError(SnowErr::Phase::Evaluator, "--every must be > 0", ctx.cmd.span);
  }

  size_t newId = ctx.snowlang.getNewTaskId();

  ctx.snowlang.scheduleTask(ScheduledTask{newId, lambda.instance, interval});

  return {static_cast<float>(newId)};
}

} // namespace Snowlang::Commands