#pragma once

#include <SFML/Graphics.hpp>

#include <cstddef>

#include "WindowManager.hpp"

class IEventListener {
public:
  virtual ~IEventListener() = default;
  virtual void handleEvent(WindowID windowId, const sf::Event &event) = 0;
};