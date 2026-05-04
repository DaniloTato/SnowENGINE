#include "UIButton.hpp"

#include "InputManager.hpp"

UIButton::UIButton(const sf::Vector2f &position, const sf::Vector2f &size,
                   const std::string &text, sf::Font &font,
                   const sf::Color &color)
    : color(color) {
  box.setPosition(position);
  box.setSize(size);
  box.setFillColor(color);

  label.setFont(font);
  label.setString(text);
  label.setCharacterSize(14);
  label.setPosition(position.x + 10, position.y + 5);
}

void UIButton::setPosition(const sf::Vector2f &p) {
  box.setPosition(p);
  label.setPosition(p.x + 10, p.y + 5);
}

void UIButton::draw(WindowManager::WindowID window) {
  WindowManager &windowManager = WindowManager::getInstance();
  sf::Vector2i mp = InputManager::getInstance().getMousePosition(window);
  bool hover =
      static_cast<float>(mp.x) >= box.getPosition().x &&
      static_cast<float>(mp.x) <= box.getPosition().x + box.getSize().x &&
      static_cast<float>(mp.y) >= box.getPosition().y &&
      static_cast<float>(mp.y) <= box.getPosition().y + box.getSize().y;

  box.setFillColor(hover ? sf::Color(color.r + 30, color.g + 30, color.b + 30)
                         : color);
  windowManager.drawOnWindow(window, box);
  windowManager.drawOnWindow(window, label);
}

bool UIButton::isClicked(const sf::Event &event,
                         WindowManager::WindowID window) {
  if (event.type != sf::Event::MouseButtonReleased)
    return false;

  if (event.mouseButton.button != sf::Mouse::Left)
    return false;

  sf::Vector2i mp = InputManager::getInstance().getMousePosition(window);

  return (static_cast<float>(mp.x) >= box.getPosition().x &&
          static_cast<float>(mp.x) <= box.getPosition().x + box.getSize().x &&
          static_cast<float>(mp.y) >= box.getPosition().y &&
          static_cast<float>(mp.y) <= box.getPosition().y + box.getSize().y);
}