#pragma once
#include "RenderProvider.hpp"
#include "TextGlyph.hpp"

class GameObject;

class TextSystem;

class TextRenderComponent : public RenderProvider {
private:
  friend class TextSystem;

  GameObject *owner;

  std::vector<TextGlyph> glyphs;

  sf::Texture *texture = nullptr;

  float animationTime = 0.f;

  float layer = 0.f;
  float paralax = 1.f;

  // Should visibleChars be part of the render component????
  size_t visibleChars = 1;

public:
  TextRenderComponent(GameObject *owner, sf::Texture *texture)
      : owner(owner), texture(texture) {}

  void appendRenderCommands(std::vector<RenderCommand> &out) const override;
};