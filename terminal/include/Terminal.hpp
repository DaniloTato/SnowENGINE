#pragma once
#include "GameText.hpp"
#include "Highlighter.hpp"
#include "IEventListener.hpp"
#include "SnowTermIO.hpp"
#include "SnowlangInstance.hpp"

class Terminal : public IEventListener {
public:
  Terminal(WindowID window, GameCamera *camera, GameCamera *tileCamera,
           Engine &engine, sf::Texture *fontTexture);

  ~Terminal() override;

  void handleEvent(WindowID windowId, const sf::Event &event) override;

  [[nodiscard]] WindowID getTargetWindow() const;

  void kill();
  void close();
  void clear();

  void update();

  [[nodiscard]] bool isOpen() const;

  static void destroyKilledTerminals(WindowManager &wm);

  [[nodiscard]] bool destroysWindowOnClose() const;

  void print(std::string_view message, std::string_view color = {});
  void printLn(std::string_view message, std::string_view color = {});

  Highlighter highlighter;

private:
  WindowID targetWindow;

  std::unordered_map<std::string, std::string> aliases;

  Snowlang::SnowTermIO snowlangIO;
  Snowlang::SnowlangInstance snowlang;

  GameText *text;

  enum class LineType : u_int8_t { Raw, Markup };

  struct TerminalLine {
    std::string text;
    LineType type = LineType::Raw;
  };

  std::deque<TerminalLine> history;
  std::vector<std::string> inputHistory;
  std::string input;
  std::string currentLine;

  static std::vector<Terminal *> s_activeTerminals;

  bool opened = true;
  bool destroyWindowOnClose = false;

  size_t cursorPos = 0;

  int historyBrowseIndex = -1;
  std::string savedInput;

private:
  void rebuildText();
  void executeSnowlang();
  void trimHistoryToFit();
  void commitLine();
  std::string escapeInput(const std::string &s);
  size_t countVisualLines(const TerminalLine &line);
};