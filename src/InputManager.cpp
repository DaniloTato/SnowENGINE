#include "InputManager.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

InputManager::InputManager() {
  for (int k = 0; k < sf::Keyboard::KeyCount; ++k) {
    currentState[(sf::Keyboard::Key)k] = false;
    previousState[(sf::Keyboard::Key)k] = false;
  }

  mouseCurrent[MouseButton::Left] = false;
  mouseCurrent[MouseButton::Right] = false;
  mouseCurrent[MouseButton::Middle] = false;

  mousePrevious = mouseCurrent;
}

sf::Keyboard::Key
InputManager::keyFromString(const std::string &keyName) const {
  static const std::unordered_map<std::string, sf::Keyboard::Key> map = {
      {"A", sf::Keyboard::A},
      {"B", sf::Keyboard::B},
      {"C", sf::Keyboard::C},
      {"D", sf::Keyboard::D},
      {"E", sf::Keyboard::E},
      {"F", sf::Keyboard::F},
      {"G", sf::Keyboard::G},
      {"H", sf::Keyboard::H},
      {"I", sf::Keyboard::I},
      {"J", sf::Keyboard::J},
      {"K", sf::Keyboard::K},
      {"L", sf::Keyboard::L},
      {"M", sf::Keyboard::M},
      {"N", sf::Keyboard::N},
      {"O", sf::Keyboard::O},
      {"P", sf::Keyboard::P},
      {"Q", sf::Keyboard::Q},
      {"R", sf::Keyboard::R},
      {"S", sf::Keyboard::S},
      {"T", sf::Keyboard::T},
      {"U", sf::Keyboard::U},
      {"V", sf::Keyboard::V},
      {"W", sf::Keyboard::W},
      {"X", sf::Keyboard::X},
      {"Y", sf::Keyboard::Y},
      {"Z", sf::Keyboard::Z},

      {"Num0", sf::Keyboard::Num0},
      {"Num1", sf::Keyboard::Num1},
      {"Num2", sf::Keyboard::Num2},
      {"Num3", sf::Keyboard::Num3},
      {"Num4", sf::Keyboard::Num4},
      {"Num5", sf::Keyboard::Num5},
      {"Num6", sf::Keyboard::Num6},
      {"Num7", sf::Keyboard::Num7},
      {"Num8", sf::Keyboard::Num8},
      {"Num9", sf::Keyboard::Num9},

      {"Space", sf::Keyboard::Space},
      {"Enter", sf::Keyboard::Enter},
      {"Escape", sf::Keyboard::Escape},
      {"LShift", sf::Keyboard::LShift},
      {"RShift", sf::Keyboard::RShift},
      {"LControl", sf::Keyboard::LControl},
      {"RControl", sf::Keyboard::RControl},

      {"KeyLeft", sf::Keyboard::Left},
      {"KeyRight", sf::Keyboard::Right},
      {"KeyUp", sf::Keyboard::Up},
      {"KeyDown", sf::Keyboard::Down}};

  auto it = map.find(keyName);
  return (it != map.end()) ? it->second : sf::Keyboard::Unknown;
}

void InputManager::bindKey(const std::string &action, sf::Keyboard::Key key) {
  bindings[action] = key;
}

void InputManager::handleEvent(const sf::Event &event) {

  if (event.type == sf::Event::KeyPressed) {
    currentState[event.key.code] = true;
  } else if (event.type == sf::Event::KeyReleased) {
    currentState[event.key.code] = false;
  }

  if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left)
      mouseCurrent[MouseButton::Left] = true;
    if (event.mouseButton.button == sf::Mouse::Right)
      mouseCurrent[MouseButton::Right] = true;
    if (event.mouseButton.button == sf::Mouse::Middle)
      mouseCurrent[MouseButton::Middle] = true;

    lastMouseClickPosition = {event.mouseButton.x, event.mouseButton.y};
  }

  else if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == sf::Mouse::Left)
      mouseCurrent[MouseButton::Left] = false;
    if (event.mouseButton.button == sf::Mouse::Right)
      mouseCurrent[MouseButton::Right] = false;
    if (event.mouseButton.button == sf::Mouse::Middle)
      mouseCurrent[MouseButton::Middle] = false;
  }

  if (event.type == sf::Event::MouseMoved) {
    lastMouseClickPosition = {event.mouseMove.x, event.mouseMove.y};
  }
}

void InputManager::update() {
  previousState = currentState;
  mousePrevious = mouseCurrent;
}

bool InputManager::isPressed(const std::string &action) const {
  auto it = bindings.find(action);
  if (it == bindings.end())
    return false;

  return currentState.at(it->second);
}

bool InputManager::isJustPressed(const std::string &action) const {
  auto it = bindings.find(action);
  if (it == bindings.end())
    return false;

  sf::Keyboard::Key key = it->second;
  return currentState.at(key) && !previousState.at(key);
}

bool InputManager::isJustReleased(const std::string &action) const {
  auto it = bindings.find(action);
  if (it == bindings.end())
    return false;

  sf::Keyboard::Key key = it->second;
  return !currentState.at(key) && previousState.at(key);
}

bool InputManager::isMousePressed(MouseButton button) const {
  return mouseCurrent.at(button);
}

bool InputManager::isMouseJustPressed(MouseButton button) const {
  return mouseCurrent.at(button) && !mousePrevious.at(button);
}

bool InputManager::isMouseJustReleased(MouseButton button) const {
  return !mouseCurrent.at(button) && mousePrevious.at(button);
}

sf::Vector2i InputManager::getMousePosition() const {
  return lastMouseClickPosition;
}

bool InputManager::loadBindingsFromJsonFile(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    std::cerr << "Cannot open bindings file: " << filePath << "\n";
    return false;
  }

  try {
    nlohmann::json j;
    file >> j;

    if (!j.contains("bindings") || !j["bindings"].is_object()) {
      std::cerr << "Invalid JSON: no 'bindings' object\n";
      return false;
    }

    for (auto &[action, keyJson] : j["bindings"].items()) {
      if (!keyJson.is_string())
        continue;

      std::string keyName = keyJson.get<std::string>();
      sf::Keyboard::Key key = keyFromString(keyName);

      if (key == sf::Keyboard::Unknown) {
        std::cerr << "Unknown key: " << keyName << "\n";
        continue;
      }

      bindKey(action, key);
    }

    return true;

  } catch (std::exception &e) {
    std::cerr << "JSON parse error: " << e.what() << "\n";
    return false;
  }
}