#pragma once
#import "GeneralContext.hpp"

struct FrameContext {
  GeneralContext &general;

  WindowID windowId;
  const sf::Event *event;
  float dt;
};