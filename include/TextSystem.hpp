#pragma once

#include "TextRenderComponent.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

class TextComponent;
class TextRenderComponent;

class TextSystem {

public:
  void update(TextComponent &text, TextRenderComponent &render, float dt);
  TextSystem();

private:
  void rebuildText(TextComponent &text, TextRenderComponent &render);

  void parseMarkup(const std::string &markup, TextComponent &text,
                   TextRenderComponent &render);

  void parseBody(const std::string &body, TextComponent &text,
                 TextRenderComponent &render);

  void pushTextChar(char c, const sf::Color &color, TextRenderComponent &render,
                    TextComponent &text, TextGlyph::Anim anim, float animParam);

  sf::Color parseColorSpec(const std::string &colorSpec);

  void rebuildLayout(TextComponent &text, TextRenderComponent &render);

  void advanceTypewriter(TextComponent &text, TextRenderComponent &render,
                         float dt);

  [[nodiscard]] float measureLineWidth(const std::vector<size_t> &lineIndices,
                                       const TextRenderComponent &render,
                                       const TextComponent &text) const;

  std::unordered_map<std::string, sf::Color> colorMap;
};