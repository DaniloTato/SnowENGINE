#pragma once
#include "WindowManager.hpp"
#include <SFML/Graphics.hpp>

class UISlider {
public:
  UISlider(const sf::Vector2f &position, float width, float minValue,
           float maxValue, float *boundValue);

  void draw(WindowManager::WindowID window);

  void handleEvent(const sf::Event &ev, WindowManager::WindowID window);

  void setValue(float v);

  float getValue() const;

  void bindTo(float *newBound);

private:
  static float clamp(float v, float a, float b) {
    return (v < a) ? a : ((v > b) ? b : v);
  }

  float getPercentage() const;

  sf::Vector2f pos;
  float width;
  float minValue;
  float maxValue;

  float *valuePtr;

  float internalValue;

  sf::RectangleShape track;
  sf::CircleShape knob;

  bool dragging = false;
};