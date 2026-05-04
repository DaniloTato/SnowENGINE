#pragma once
#include "WindowManager.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <unordered_map>

enum class MouseButton : std::uint8_t { Left, Right, Middle };

class InputManager {
public:
  static InputManager &getInstance() {
    static InputManager instance;
    return instance;
  }

  void handleEvent(WindowManager::WindowID window, const sf::Event &event);
  void update();

  [[nodiscard]] bool isPressed(const std::string &action) const;
  [[nodiscard]] bool isJustPressed(const std::string &action) const;
  [[nodiscard]] bool isJustReleased(const std::string &action) const;

  bool loadBindingsFromJsonFile(const std::string &filePath);
  void bindKey(const std::string &action, sf::Keyboard::Key key);

  [[nodiscard]] bool isMousePressed(MouseButton button) const;
  [[nodiscard]] bool isMouseJustPressed(MouseButton button) const;
  [[nodiscard]] bool isMouseJustReleased(MouseButton button) const;

  [[nodiscard]] sf::Vector2i getMousePosition(WindowManager::WindowID id) const;
  [[nodiscard]] sf::Keyboard::Key
  keyFromString(const std::string &keyName) const;

  InputManager(const InputManager &) = delete;
  InputManager &operator=(const InputManager &) = delete;

  InputManager(InputManager &&) = delete;
  InputManager &operator=(InputManager &&) = delete;

private:
  InputManager();

  std::unordered_map<sf::Keyboard::Key, bool> currentState;
  std::unordered_map<sf::Keyboard::Key, bool> previousState;

  std::unordered_map<MouseButton, bool> mouseCurrent;
  std::unordered_map<MouseButton, bool> mousePrevious;

  std::unordered_map<WindowManager::WindowID, sf::Vector2i> mousePositions;

  sf::Vector2i lastMouseClickPosition;

  std::unordered_map<std::string, sf::Keyboard::Key> bindings;
};