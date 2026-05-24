#pragma once

#include "WindowID.hpp"

class Engine;

namespace SceneBuilder {
struct SceneContext {
  Engine *engine;
  WindowID mainWindow;
};
} // namespace SceneBuilder