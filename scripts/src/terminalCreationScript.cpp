#include "terminalCreationScript.hpp"

#include "GameState.hpp"
#include "InputManager.hpp"
#include "Terminal.hpp"
#include "TextureManager.hpp"

namespace Scripts {

static Terminal *s_terminal = nullptr;

void terminalCreationScript(ScriptRunner &renderable,
                            const GeneralContext &ctx) {

  if (InputManager::getInstance().isJustPressed("terminal")) {
    if (!s_terminal) {
      GameState::getInstance().createWindow(WindowTypes::TERMINAL, 900, 600,
                                            "Snowgun Terminal");
      GameState::getInstance().createCamera(CameraTypes::TERMINAL,
                                            {WindowTypes::TERMINAL});
      s_terminal = new Terminal(GameState::getInstance().getTerminalWindow(),
                                GameState::getInstance().getTerminalCamera(),
                                &TextureManager::getInstance().get("snowFont"));
    }
  }

  if (s_terminal) {
    sf::Event event;
    while (s_terminal->getTargetWindow()->pollEvent(event)) {

      if (event.type == sf::Event::Closed) {
        if (s_terminal->isOpen()) {
          s_terminal->close();
          continue;
        }
      }

      s_terminal->handleEvent(event);
    }

    s_terminal->update();

    if (!s_terminal->isOpen()) {
      s_terminal = nullptr;
    }
  }
}

} // namespace Scripts