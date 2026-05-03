#include "Commands.hpp"
#include "Constants.hpp"
#include "GameState.hpp"
#include "LevelManager.hpp"
#include "RuntimeValue.hpp"

namespace Snowlang::Commands {

RuntimeValue tileCommand(const CommandContext &ctx) {

  const auto &span = ctx.cmd.span;

  if (!ctx.hasFlag("at")) {
    throwError(SnowErr::Phase::Evaluator, "no tile position specified. (use --at flag).",
               ctx.cmd.span);
  }

  auto [x, y] = SnowlangHelper::GetList<float, float>(span)(
      RuntimeValue(ctx.getFlag<RuntimeValue::List>("at", RuntimeValue::List{})));

  auto [Rx, Ry] = SnowlangHelper::GetList<float, float>(span)(ctx.getFlag<RuntimeValue::List>(
      "rect", RuntimeValue::List{RuntimeValue(0.f), RuntimeValue(0.f)}));

  const auto layer =
      SnowlangHelper::RuntimeValueTo<float>(span)(ctx.getFlag<float>("layer", float(0)));

  if (ctx.hasFlag("destroy")) {
    LevelManager::getInstance().queueDeleteTile(static_cast<int>(layer), static_cast<int>(x),
                                                static_cast<int>(y));
  } else {
    // MUST CHANGE. TIGHTLY COUPLED
    LevelManager::getInstance().queueCreateTile(
        WindowManager::getInstance().getMain(), GameState::getInstance().getMainCamera(),
        static_cast<int>(layer), static_cast<int>(x), static_cast<int>(y),
        sf::IntRect({
            static_cast<int>(Rx),
            static_cast<int>(Ry),
            Constants::TILE_SIZE,
            Constants::TILE_SIZE,
        }));
  }

  return {true};
}

} // namespace Snowlang::Commands
