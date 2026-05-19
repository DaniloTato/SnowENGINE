#include "terminalCreationScript.hpp"

#include "InputManager.hpp"
#include "Terminal.hpp"
#include "TextureManager.hpp"

#include "RegistryMacros.hpp"
#include "ScriptRegistry.hpp" // IWYU pragma: keep
#include "Scripter.hpp"

namespace Scripts {

static Terminal *s_terminal = nullptr;

void terminalCreationScript(ScriptRunner &renderable,
                            const GeneralContext &ctx) {

  if (InputManager::getInstance().isJustPressed("terminal")) {

    if (!s_terminal) {

      WindowManager &windowManager = ctx.engine->getWindowManager();

      const WindowID terminalWindow = windowManager.create(
          WindowManager::Set::TERMINAL, 900, 500, "Snowgun Terminal");

      const CameraID terminalCamera = ctx.cameraManager->createCamera(
          GameObject::UpdateDomain(WindowManager::Set::TERMINAL), true);

      s_terminal = new Terminal(terminalWindow,
                                ctx.cameraManager->getCamera(terminalCamera),
                                ctx.cameraManager->getCamera(ctx.mainCamera),
                                ctx.engine->getWindowManager(),
                                &TextureManager::getInstance().get("snowFont"));

      windowManager.subscribe(terminalWindow, &InputManager::getInstance());
      windowManager.subscribe(terminalWindow, s_terminal);
    }
  }

  if (s_terminal) {

    s_terminal->update();

    if (!s_terminal->isOpen()) {
      s_terminal = nullptr;
    }
  }
}

} // namespace Scripts

REGISTER_SCRIPT("terminalCreationScript", Scripts::terminalCreationScript,
                ScriptRunner);