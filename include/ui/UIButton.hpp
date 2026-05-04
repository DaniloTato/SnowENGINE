#pragma once
#include "WindowManager.hpp"
#include <SFML/Graphics.hpp>

class UIButton {
public:
  UIButton(const sf::Vector2f &position, const sf::Vector2f &size,
           const std::string &text, sf::Font &font,
           const sf::Color &color = sf::Color(60, 60, 60));

  void draw(WindowManager::WindowID window);
  bool isClicked(const sf::Event &event, WindowManager::WindowID window);

  void setText(const std::string &t) { label.setString(t); }
  void setPosition(const sf::Vector2f &p);

private:
  sf::RectangleShape box;
  sf::Text label;
  sf::Color color;
};