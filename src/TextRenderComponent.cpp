#include "TextRenderComponent.hpp"

#include "GameObject.hpp"

#include <cmath>

void TextRenderComponent::appendRenderCommands(
    std::vector<RenderCommand> &out) const {

  for (const auto &glyph : glyphs) {

    if (glyph.appearIndex >= 0 &&
        static_cast<size_t>(glyph.appearIndex) >= visibleChars) {
      continue;
    }

    if (glyph.c == '\n') {
      continue;
    }

    sf::Vector2f pos = glyph.basePos;

    switch (glyph.anim) {

    case TextGlyph::Anim::Sin: {

      float amplitude = glyph.animParam > 0.f ? glyph.animParam : 4.f;

      constexpr float frequency = 6.f;

      pos.y += std::sin(animationTime * frequency + glyph.phase) * amplitude;

      break;
    }

    case TextGlyph::Anim::Shake: {

      float intensity = glyph.animParam > 0.f ? glyph.animParam : 2.f;

      float jitterX =
          (std::sin(animationTime * 40.f + glyph.phase) * 0.5f +
           std::sin(animationTime * 23.f + glyph.phase * 1.3f) * 0.5f) *
          intensity;

      float jitterY = std::sin(animationTime * 30.3f + glyph.phase * 0.7f) *
                      0.6f * intensity;

      pos.x += jitterX;
      pos.y += jitterY;

      break;
    }

    case TextGlyph::Anim::None:
    default:
      break;
    }

    out.push_back({
        .texture = texture,
        .position = owner->position + pos,
        .textureRect = glyph.texRect,
        .color = glyph.color,
        .scale = {1.f, 1.f},
        .layer = layer,
        .paralax = paralax,
    });
  }
}