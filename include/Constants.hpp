#pragma once

#include "SFML/System/Vector2.hpp"

#include "GameText.hpp"

namespace Constants {

inline constexpr int TILE_SIZE = 16;
inline constexpr int COLLISION_GRID_WIDTH = 4;
inline constexpr int COLLISION_GRID_HEIGHT = 4;
inline constexpr int SCREEN_WIDTH = 800;
inline constexpr int SCREEN_HEIGHT = 600;
inline constexpr int FRAME_RATE = 60;
inline constexpr int PARTICLES_LAYER = -1;
inline constexpr int EFFECTS_LAYER = -99;
inline constexpr int TEXT_LAYER = -100;
inline constexpr int UI_LAYER = -101;
inline constexpr int UI_TEXT_LAYER = -105;
inline constexpr int OVERLAY_LAYER = -200;
inline constexpr const char *MAIN_WINDOW_NAME = "SnowgunWONDERS";
const sf::Vector2f STARTING_PLAYER_CAMERA_RELATION = {20, -30};
const GameText::FontAtlas DEFAULT_FONT_ATLAS = {
    .glyphW = 9, .glyphH = 10, .cols = 98, .firstChar = 32};

} // namespace Constants