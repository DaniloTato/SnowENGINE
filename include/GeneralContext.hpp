#pragma once

#include "RenderizerParameters.hpp"
#include "TangibleObject.hpp"
#include <SFML/System/Vector2.hpp>

struct GeneralContext {
  sf::Vector2f *playerPosition;
  RenderizerParameters *textParams;
  TangibleObject *player;
  sf::Texture *bulletTexture;
};

struct EmptyContext : public GeneralContext {
  EmptyContext()
      : GeneralContext{.playerPosition = nullptr,
                       .textParams = nullptr,
                       .player = nullptr,
                       .bulletTexture = nullptr} {}
};