#include "terminalCreationScript.hpp"

#include "GameState.hpp"
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

      std::cout << "creating terminal\n";

      const WindowID id = WindowManager::getInstance().create(
          WindowManager::Set::TERMINAL, 900, 500, "Snowgun Terminal");

      GameState::getInstance().createCamera(
          CameraTypes::TERMINAL,
          GameObject::UpdateDomain(WindowManager::Set::TERMINAL));

      s_terminal =
          new Terminal(id, GameState::getInstance().getTerminalCamera(),
                       &TextureManager::getInstance().get("snowFont"));
      WindowManager::getInstance().subscribe(id, &InputManager::getInstance());
      WindowManager::getInstance().subscribe(id, s_terminal);
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