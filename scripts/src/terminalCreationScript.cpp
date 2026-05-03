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
      std::cout << "creating terminal\n";
      const WindowManager::WindowID id = WindowManager::getInstance().create(
          WindowManager::Set::TERMINAL, 900, 500, "Snowgun Terminal");
      GameState::getInstance().createCamera(
          CameraTypes::TERMINAL,
          GameObject::UpdateDomain(WindowManager::Set::TERMINAL));
      s_terminal =
          new Terminal(id, GameState::getInstance().getTerminalCamera(),
                       &TextureManager::getInstance().get("snowFont"));
    }
  }

  if (s_terminal) {
    sf::Event event;
    while (WindowManager::getInstance().pollEventOnWindow(
        s_terminal->getTargetWindow(), event)) {

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