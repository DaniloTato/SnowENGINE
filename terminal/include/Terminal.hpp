#pragma once

#include "Highlighter.hpp"
#include "IEventListener.hpp"
#include "SnowTermIO.hpp"
#include "SnowlangInstance.hpp"

#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

class Terminal : public IEventListener {
public:
  Terminal();
  ~Terminal() override = default;

  void handleEvent(WindowID windowId, const sf::Event &event) override;

  void update();

  void kill();
  void close();
  void clear();

  [[nodiscard]]
  bool isOpen() const;

  void print(std::string_view message, std::string_view color = {});

  void printLn(std::string_view message, std::string_view color = {});

  [[nodiscard]]
  std::string buildMarkup() const;

  Highlighter highlighter;

private:
  enum class LineType : uint8_t { Raw, Markup };

  struct TerminalLine {
    std::string text;
    LineType type = LineType::Raw;
  };

private:
  Snowlang::SnowTermIO snowlangIO;
  Snowlang::SnowlangInstance snowlang;

  std::unordered_map<std::string, std::string> aliases;

  std::deque<TerminalLine> history;
  std::vector<std::string> inputHistory;

  std::string input;
  std::string currentLine;

  bool opened = true;
  bool destroyWindowOnClose = false;

  size_t cursorPos = 0;

  int historyBrowseIndex = -1;
  std::string savedInput;

private:
  void executeSnowlang();
  void trimHistoryToFit();
  void commitLine();

  [[nodiscard]] std::string escapeInput(const std::string &s) const;

  [[nodiscard]] size_t countVisualLines(const TerminalLine &line) const;
};