#include "UISlider.hpp"
#include "InputManager.hpp"

UISlider::UISlider(const sf::Vector2f &position, float width, float minValue,
                   float maxValue, float *boundValue)
    : pos(position), width(width), minValue(minValue), maxValue(maxValue),
      valuePtr(boundValue), internalValue(boundValue ? *boundValue : minValue) {
  // track
  track.setPosition(pos);
  track.setSize({width, 8.f});
  track.setFillColor(sf::Color(80, 80, 80));

  // knob
  knob.setRadius(6.f);
  knob.setOrigin(6.f, 6.f); // center origin
  knob.setFillColor(sf::Color(200, 200, 200));

  // place knob according to initial value
  setValue(internalValue);
}

void UISlider::bindTo(float *newBound) {
  valuePtr = newBound;
  if (valuePtr) {
    // synchronize internal value with external
    internalValue = *valuePtr;
  }
  setValue(internalValue);
}

float UISlider::getPercentage() const {
  float v = (valuePtr ? *valuePtr : internalValue);
  if (maxValue == minValue)
    return 0.f;
  float pct = (v - minValue) / (maxValue - minValue);
  return clamp(pct, 0.f, 1.f);
}

void UISlider::setValue(float v) {
  v = clamp(v, minValue, maxValue);
  internalValue = v;
  if (valuePtr) {
    *valuePtr = v;
  }

  // move knob to the proper screen position
  float pct = getPercentage();
  float knobX = pos.x + pct * width;
  knob.setPosition(knobX, pos.y + track.getSize().y / 2.f);
}

float UISlider::getValue() const {
  return (valuePtr ? *valuePtr : internalValue);
}

void UISlider::draw(WindowID window, WindowManager &windowManager) {
  // make sure knob follows current value
  setValue(getValue());

  windowManager.drawOnWindow(window, track);
  windowManager.drawOnWindow(window, knob);
}

void UISlider::handleEvent(const sf::Event &ev, WindowID window) {
  sf::Vector2i mp = InputManager::getInstance().getMousePosition(window);

  // start dragging if press on knob (distance threshold)
  if (ev.type == sf::Event::MouseButtonPressed &&
      ev.mouseButton.button == sf::Mouse::Left) {
    float dx = float(mp.x) - knob.getPosition().x;
    float dy = float(mp.y) - knob.getPosition().y;
    if (dx * dx + dy * dy <=
        (knob.getRadius() + 2.f) * (knob.getRadius() + 2.f)) {
      dragging = true;
      return;
    }

    // also allow clicking on the track to jump there
    sf::FloatRect trackRect(track.getPosition(), track.getSize());
    if (trackRect.contains(static_cast<float>(mp.x),
                           static_cast<float>(mp.y))) {
      float newPct = (static_cast<float>(mp.x) - pos.x) / width;
      newPct = clamp(newPct, 0.f, 1.f);
      float newVal = minValue + newPct * (maxValue - minValue);
      setValue(newVal);
      return;
    }
  }

  // stop dragging
  if (ev.type == sf::Event::MouseButtonReleased &&
      ev.mouseButton.button == sf::Mouse::Left) {
    dragging = false;
  }

  // if dragging, update position from current mouse
  if (dragging && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
    float newPct = (static_cast<float>(mp.x) - pos.x) / width;
    newPct = clamp(newPct, 0.f, 1.f);
    float newVal = minValue + newPct * (maxValue - minValue);
    setValue(newVal);
  }
}