#pragma once

#include "IEventListener.hpp"
#include "WindowManager.hpp"

class WindowLifecycleListener : public IEventListener {
  WindowManager &wm;

public:
  WindowLifecycleListener(WindowManager &windowManager) : wm(windowManager) {}

  void handleEvent(WindowID id, const sf::Event &event) override {

    switch (event.type) {

    case sf::Event::LostFocus:
      wm.pauseWindow(id);
      break;

    case sf::Event::GainedFocus:
      wm.resumeWindow(id);
      break;

    case sf::Event::Closed:
      wm.queueDestroy(id);
      break;

    default:
      break;
    }
  }
};