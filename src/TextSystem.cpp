#include "TextSystem.hpp"

#include "ColorPalette.hpp"
#include "TextComponent.hpp"
#include "TextRenderComponent.hpp"
#include <sstream>

#include <iostream>

namespace {

std::string trimLeadingSpace(std::string s) {
  while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) {
    s.erase(s.begin());
  }
  return s;
}

struct TextAttributes {

  sf::Color color;
  TextGlyph::Anim anim;
  float animParam;
};

} // namespace

TextSystem::TextSystem() {

  colorMap["white"] = sf::Color::White;
  colorMap["black"] = sf::Color::Black;

  colorMap["red"] = ColorPalette::MexicanPink;
  colorMap["green"] = sf::Color::Green;

  colorMap["blue"] = ColorPalette::ElectricBlue;
  colorMap["lightblue"] = ColorPalette::LightBlue;

  colorMap["yellow"] = ColorPalette::LimeGreen;

  colorMap["cyan"] = sf::Color::Cyan;
  colorMap["magenta"] = sf::Color::Magenta;

  colorMap["purple"] = ColorPalette::VividIndigo;

  colorMap["transparent"] = sf::Color(0, 0, 0, 0);
}

void TextSystem::update(TextComponent &text, TextRenderComponent &render,
                        float dt) {

  if (text.dirty) {
    rebuildText(text, render);
    text.dirty = false;
  }

  render.animationTime += dt;

  advanceTypewriter(text, render, dt);
}

void TextSystem::rebuildText(TextComponent &text, TextRenderComponent &render) {

  render.glyphs.clear();

  text.lines.clear();

  text.visibleChars = 0;
  text.revealableCount = 0;
  text.typeTimer = 0.f;

  render.animationTime = 0.f;

  parseMarkup(text.markup, text, render);

  rebuildLayout(text, render);

  if (text.effect == TextEffect::Typewriter) {

    text.visibleChars = 0;
    render.visibleChars = 0;

  } else {

    text.visibleChars = text.revealableCount;
    render.visibleChars = text.revealableCount;
  }
}

void TextSystem::parseMarkup(const std::string &markup, TextComponent &text,
                             TextRenderComponent &render) {

  std::istringstream stream(markup);

  std::string line;
  std::string body;

  bool inHeader = true;

  while (std::getline(stream, line)) {

    if (line.empty()) {
      continue;
    }

    if (inHeader) {

      std::string trimmed = trimLeadingSpace(line);

      if (!trimmed.empty() && trimmed.front() == '#') {

        std::string header = trimmed.substr(1);

        std::istringstream tokens(header);

        std::string key;
        tokens >> key;

        if (key == "effect") {

          std::string effectName;
          tokens >> effectName;

          if (effectName == "typewriter") {

            text.effect = TextEffect::Typewriter;

            float speed;

            if (tokens >> speed) {
              text.typeDelay = speed;
            }

          } else {

            text.effect = TextEffect::None;
          }
        }

        else if (key == "alignment") {

          std::string alignment;
          tokens >> alignment;

          std::ranges::transform(
              alignment, alignment.begin(),
              [](unsigned char c) { return std::tolower(c); });

          if (alignment == "center") {
            text.alignment = TextAlign::Center;
          }

          else if (alignment == "right") {
            text.alignment = TextAlign::Right;
          }

          else {
            text.alignment = TextAlign::Left;
          }
        }

        else if (key == "boundary") {

          float boundary;

          if (tokens >> boundary) {
            text.boundary = boundary;
          }
        }

        else if (key == "position") {

          float x;
          float y;

          if (tokens >> x >> y) {
            text.origin = {x, y};
          }
        }

        continue;
      }

      inHeader = false;
    }

    body += line;
    body += '\n';
  }

  parseBody(body, text, render);
}

void TextSystem::parseBody(const std::string &body, TextComponent &text,
                           TextRenderComponent &render) {

  std::vector<TextAttributes> stack;

  stack.push_back({
      sf::Color::White,
      TextGlyph::Anim::None,
      0.f,
  });

  int appearCounter = 0;

  for (size_t i = 0; i < body.size();) {

    if (body[i] == '\\' && i + 1 < body.size()) {

      pushTextChar(body[i + 1], stack.back().color, render, text,
                   stack.back().anim, stack.back().animParam);

      render.glyphs.back().appearIndex = appearCounter++;

      i += 2;
      continue;
    }

    if (body[i] == '[') {

      size_t end = body.find(']', i + 1);

      if (end == std::string::npos) {

        pushTextChar('[', stack.back().color, render, text, stack.back().anim,
                     stack.back().animParam);

        render.glyphs.back().appearIndex = appearCounter++;

        ++i;
        continue;
      }

      std::string tag = body.substr(i + 1, end - i - 1);

      while (!tag.empty() && std::isspace((unsigned char)tag.front())) {
        tag.erase(tag.begin());
      }

      while (!tag.empty() && std::isspace((unsigned char)tag.back())) {
        tag.pop_back();
      }

      if (tag == "ln") {

        pushTextChar('\n', stack.back().color, render, text, stack.back().anim,
                     stack.back().animParam);
      }

      else if (!tag.empty() && tag.front() == '/') {

        if (stack.size() > 1) {
          stack.pop_back();
        }
      }

      else if (tag.starts_with("color")) {

        size_t eq = tag.find('=');

        if (eq != std::string::npos) {

          sf::Color color = parseColorSpec(tag.substr(eq + 1));

          stack.push_back({
              color,
              stack.back().anim,
              stack.back().animParam,
          });
        }
      }

      else if (tag.starts_with("anim")) {

        size_t eq = tag.find('=');

        if (eq != std::string::npos) {

          std::string spec = tag.substr(eq + 1);

          std::string name = spec;

          float param = 0.f;

          size_t colon = spec.find(':');

          if (colon != std::string::npos) {

            name = spec.substr(0, colon);

            param = std::stof(spec.substr(colon + 1));
          }

          TextGlyph::Anim anim = TextGlyph::Anim::None;

          if (name == "sin") {
            anim = TextGlyph::Anim::Sin;
          }

          else if (name == "shake") {
            anim = TextGlyph::Anim::Shake;
          }

          stack.push_back({
              stack.back().color,
              anim,
              param,
          });
        }
      }

      i = end + 1;
      continue;
    }

    if (body[i] == '\n') {

      pushTextChar('\n', stack.back().color, render, text, stack.back().anim,
                   stack.back().animParam);

      ++i;
      continue;
    }

    pushTextChar(body[i], stack.back().color, render, text, stack.back().anim,
                 stack.back().animParam);

    render.glyphs.back().appearIndex = appearCounter++;

    ++i;
  }
}

void TextSystem::pushTextChar(char c, const sf::Color &color,
                              TextRenderComponent &render, TextComponent &text,
                              TextGlyph::Anim anim, float animParam) {

  TextGlyph glyph;

  glyph.c = c;

  int code = static_cast<unsigned char>(c);

  if (code < text.fontAtlas.firstChar) {

    glyph.texRect = {
        0,
        0,
        text.fontAtlas.glyphW,
        text.fontAtlas.glyphH,
    };
  }

  else {

    int index = code - text.fontAtlas.firstChar;

    int col = index % text.fontAtlas.cols;

    int row = index / text.fontAtlas.cols;

    glyph.texRect = {
        col * text.fontAtlas.glyphW,
        row * text.fontAtlas.glyphH,
        text.fontAtlas.glyphW,
        text.fontAtlas.glyphH,
    };
  }

  glyph.color = color;
  glyph.anim = anim;
  glyph.animParam = animParam;

  glyph.phase = static_cast<float>(render.glyphs.size()) * 0.2f;

  glyph.appearIndex = -1;

  render.glyphs.push_back(glyph);
}

sf::Color TextSystem::parseColorSpec(const std::string &colorSpec) {

  std::string value = colorSpec;

  while (!value.empty() && std::isspace((unsigned char)value.front())) {
    value.erase(value.begin());
  }

  while (!value.empty() && std::isspace((unsigned char)value.back())) {
    value.pop_back();
  }

  std::ranges::transform(value, value.begin(),
                         [](unsigned char c) { return std::tolower(c); });

  if (value.empty()) {
    return sf::Color::White;
  }

  if (value.front() == '#') {

    if (value.size() >= 7) {

      unsigned int r = 0;
      unsigned int g = 0;
      unsigned int b = 0;

      sscanf(value.c_str() + 1, "%02x%02x%02x", &r, &g, &b);

      return {
          static_cast<sf::Uint8>(r),
          static_cast<sf::Uint8>(g),
          static_cast<sf::Uint8>(b),
      };
    }
  }

  auto it = colorMap.find(value);

  if (it != colorMap.end()) {
    return it->second;
  }

  if (value.size() == 6) {

    unsigned int r = 0;
    unsigned int g = 0;
    unsigned int b = 0;

    sscanf(value.c_str(), "%02x%02x%02x", &r, &g, &b);

    return {
        static_cast<sf::Uint8>(r),
        static_cast<sf::Uint8>(g),
        static_cast<sf::Uint8>(b),
    };
  }

  return sf::Color::White;
}

float TextSystem::measureLineWidth(const std::vector<size_t> &lineIndices,
                                   const TextRenderComponent &render,
                                   const TextComponent &text) const {

  return static_cast<float>(lineIndices.size()) *
         static_cast<float>(text.fontAtlas.glyphW);
}

void TextSystem::rebuildLayout(TextComponent &text,
                               TextRenderComponent &render) {

  text.lines.clear();

  if (render.glyphs.empty()) {
    text.revealableCount = 0;
    return;
  }

  std::vector<size_t> currentLine;
  float currentWidth = 0.f;

  auto pushLine = [&](std::vector<size_t> &line) {
    if (!line.empty()) {
      text.lines.push_back(line);
      line.clear();
    } else {
      text.lines.emplace_back();
    }
  };

  for (size_t i = 0; i < render.glyphs.size(); ++i) {

    auto &glyph = render.glyphs[i];

    if (glyph.c == '\n') {

      pushLine(currentLine);
      currentWidth = 0.f;
      continue;
    }

    float charWidth = static_cast<float>(text.fontAtlas.glyphW);

    if (text.boundary < 100000.f && currentWidth + charWidth > text.boundary &&
        !currentLine.empty()) {

      pushLine(currentLine);
      currentWidth = 0.f;
    }

    currentLine.push_back(i);
    currentWidth += charWidth;
  }

  pushLine(currentLine);

  float lineY = 0.f;

  for (const auto &line : text.lines) {

    float lineWidth = measureLineWidth(line, render, text);

    float startX = text.origin.x;

    switch (text.alignment) {

    case TextAlign::Left:
      break;

    case TextAlign::Center:
      startX = text.origin.x + (text.boundary - lineWidth) * 0.5f;
      break;

    case TextAlign::Right:
      startX = text.origin.x + (text.boundary - lineWidth);
      break;
    }

    float cursorX = startX;

    for (size_t glyphIndex : line) {

      render.glyphs[glyphIndex].basePos = {cursorX, text.origin.y + lineY};

      cursorX += static_cast<float>(text.fontAtlas.glyphW);
    }

    lineY += static_cast<float>(text.fontAtlas.glyphH) + text.lineSpacing;
  }

  text.revealableCount = 0;

  for (auto &glyph : render.glyphs) {

    if (glyph.c != '\n') {

      glyph.appearIndex = static_cast<int>(text.revealableCount);

      ++text.revealableCount;
    } else {

      glyph.appearIndex = -1;
    }
  }

  if (text.effect == TextEffect::None) {

    text.visibleChars = text.revealableCount;
  } else {

    text.visibleChars = std::min(text.visibleChars, text.revealableCount);
  }
}

void TextSystem::advanceTypewriter(TextComponent &text,
                                   TextRenderComponent &render, float dt) {

  if (text.effect != TextEffect::Typewriter) {
    return;
  }

  text.typeTimer += dt;

  while (text.typeTimer >= text.typeDelay &&
         text.visibleChars < text.revealableCount) {

    text.typeTimer -= text.typeDelay;
    ++text.visibleChars;
    render.visibleChars = text.visibleChars;
  }
}