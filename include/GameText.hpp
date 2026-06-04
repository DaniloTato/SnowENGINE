#pragma once
#include "GameObject.hpp"
#include "OldRenderCommand.hpp"
#include "PolyRenderizer.hpp"
#include "RenderizerParameters.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>

/*NEEDS REFACTORING*/

class GameText : public GameObject {
public:
  struct FontAtlas {
    int glyphW;
    int glyphH;
    int cols;
    int firstChar;
  };

  struct Glyph {
    char c;
    sf::IntRect texRect;
    sf::Vector2f basePos;
    sf::Color color;
    enum Anim : std::uint8_t { None, Sin, Shake } anim = None;
    float animParam =
        0.f;              // for shake intensity or sin amplitude/freq if needed
    float phase = 0.f;    // per-glyph phase for wave offset
    int appearIndex = -1; // index order for typewriter reveal
  };

  enum class Align : std::uint8_t { Left, Center, Right };

  enum class Effect : std::uint8_t { None, Typewriter };

  GameText(RenderizerParameters params);

  void update(const GeneralContext &ctx) override;

  void setFontAtlas(FontAtlas atlas);

  // Load text using "markup" string. Header directives are processed from lines
  // starting '#'. Example header lines: #effect typewriter 0.04 #position 30 50
  // #boundary 300
  // #soundpool {"s1.wav","s2.wav"}
  // #alignment center
  void loadFromMarkup(const std::string &markup);

  void setPosition(const sf::Vector2f &p) {
    origin = p;
    rebuildLayout();
  }
  void setBoundary(float rightBoundary) {
    boundary = rightBoundary;
    rebuildLayout();
  }
  void setAlignment(Align a) {
    alignment = a;
    rebuildLayout();
  }
  void setTypewriter(bool enabled, float delayPerChar = 0.05f) {
    effect = enabled ? Effect::Typewriter : Effect::None;
    typeDelay = delayPerChar;
    resetTypewriter();
  }
  void
  setSoundPool(const std::vector<std::string> &files); // loads soundbuffers

  void resetTypewriter();
  void forceRevealAll();

  void updateRenderCommandBuffer();

  [[nodiscard]] size_t totalChars() const { return glyphs.size(); }
  [[nodiscard]] size_t visibleChars() const { return revealedCount; }

public:
  PolyRenderizer renderizer;

private:
  void parseHeaderLine(const std::string &line);
  void parseBody(const std::string &body);
  void pushTextChar(char c, const sf::Color &color, Glyph::Anim anim,
                    float animParam);
  [[nodiscard]] sf::Color parseColorSpec(const std::string &s) const;
  void rebuildLayout();
  [[nodiscard]] float
  measureLineWidth(const std::vector<size_t> &lineIndices) const;

  void advanceTypewriter();
  void playTypeSound();

  FontAtlas fontAtlas;
  int lineSpacing;

  sf::Vector2f origin = {0.f, 0.f};
  float boundary = 10000.f;
  Align alignment = Align::Left;

  Effect effect = Effect::None;
  float typeDelay = 0.04f;
  size_t revealedCount = 0;
  float typeTimer = 0.f;

  std::vector<sf::SoundBuffer> soundBuffers;
  size_t soundPlayIndex = 0;
  bool useSoundPool = false;
  bool playSoundOnChars = true;

  std::vector<Glyph> glyphs;
  std::vector<std::vector<size_t>> lines;

  float globalTime = 0.f;

  size_t revealableCount = 0;

  std::vector<OldRenderCommand> renderCommandBuffer;

  std::map<std::string, sf::Color> colorNameMap;
};