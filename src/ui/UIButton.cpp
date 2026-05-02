#include "UIButton.hpp"

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

void UIButton::draw(sf::RenderWindow &window) {
  sf::Vector2i mp = sf::Mouse::getPosition(window);
  bool hover =
      static_cast<float>(mp.x) >= box.getPosition().x &&
      static_cast<float>(mp.x) <= box.getPosition().x + box.getSize().x &&
      static_cast<float>(mp.y) >= box.getPosition().y &&
      static_cast<float>(mp.y) <= box.getPosition().y + box.getSize().y;

  box.setFillColor(hover ? sf::Color(color.r + 30, color.g + 30, color.b + 30)
                         : color);
  window.draw(box);
  window.draw(label);
}

bool UIButton::isClicked(const sf::Event &event, sf::RenderWindow &window) {
  if (event.type != sf::Event::MouseButtonReleased)
    return false;

  if (event.mouseButton.button != sf::Mouse::Left)
    return false;

  sf::Vector2i mp = sf::Mouse::getPosition(window);

  return (static_cast<float>(mp.x) >= box.getPosition().x &&
          static_cast<float>(mp.x) <= box.getPosition().x + box.getSize().x &&
          static_cast<float>(mp.y) >= box.getPosition().y &&
          static_cast<float>(mp.y) <= box.getPosition().y + box.getSize().y);
}