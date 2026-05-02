#pragma once

#include <SFML/Graphics.hpp>
#include <array>

namespace ColorPalette {

inline const sf::Color ElectricBlue = sf::Color(0, 0, 252);
inline const sf::Color LightBlue = sf::Color(60, 188, 252);
inline const sf::Color LimeGreen = sf::Color(184, 248, 24);
inline const sf::Color Cyan = sf::Color(0, 252, 252);
inline const sf::Color HotPink = sf::Color(248, 120, 248);
inline const sf::Color SoftViolet = sf::Color(152, 120, 248);
inline const sf::Color MexicanPink = sf::Color(228, 0, 88);
inline const sf::Color Black = sf::Color(0, 0, 0);
inline const sf::Color White = sf::Color(252, 252, 252);
inline const sf::Color DarkCyanBlue = sf::Color(0, 64, 88);
inline const sf::Color VividIndigo = sf::Color(104, 68, 252);
inline const sf::Color NeonMagenta = sf::Color(216, 0, 204);
inline const sf::Color DeepMagenta = sf::Color(148, 0, 132);
inline const sf::Color PeachCream = sf::Color(252, 224, 168);

inline constexpr std::array<const sf::Color *, 6> EXPLOSION_COLORS = {
    &HotPink, &SoftViolet, &LimeGreen, &Cyan, &White, &MexicanPink,
};

} // namespace ColorPalette