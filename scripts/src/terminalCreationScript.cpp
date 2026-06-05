#include "terminalCreationScript.hpp"

#include "InputManager.hpp"

#include "RegistryMacros.hpp"
#include "ScriptRegistry.hpp" // IWYU pragma: keep
#include "ScriptRunner.hpp"
#include "Scripter.hpp"

namespace Scripts {

void terminalCreationScript(ScriptRunner &renderable,
                            const GeneralContext &ctx) {
  if (InputManager::getInstance().isJustPressed("terminal")) {
    ctx.engine->getSceneManager().openScene("terminal", "terminalScene",
                                            Scene::Context{
                                                .engine = ctx.engine,
                                                .mainWindow = ctx.mainWindow,
                                            });
  }
}

} // namespace Scripts

REGISTER_SCRIPT("terminalCreationScript", Scripts::terminalCreationScript,
                ScriptRunner);