#pragma once
#import "GeneralContext.hpp"

struct FrameContext {
  GeneralContext &general;

  WindowManager::WindowID windowId;
  const sf::Event *event;
  float dt;
};