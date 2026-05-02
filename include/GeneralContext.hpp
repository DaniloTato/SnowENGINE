#pragma once

#include "TangibleObject.hpp"
#include <SFML/System/Vector2.hpp>

struct GeneralContext {
  TangibleObject *player;
};

struct EmptyContext : public GeneralContext {
  EmptyContext()
      : GeneralContext{
            .player = nullptr,
        } {}
};