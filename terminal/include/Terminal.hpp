#pragma once
#include "GameCamera.hpp"
#include "GameText.hpp"
#include "Highlighter.hpp"
#include "SnowTermIO.hpp"
#include "SnowlangInstance.hpp"

class Terminal {
public:
  Terminal(WindowManager::WindowID window, GameCamera *camera,
           sf::Texture *fontTexture);
  ~Terminal();

  void handleEvent(const sf::Event &event);

  [[nodiscard]] WindowManager::WindowID getTargetWindow() const;

  void kill();
  void close();
  void clear();

  void update();

  [[nodiscard]] bool isOpen() const;

  static void destroyKilledTerminals();

  [[nodiscard]] bool destroysWindowOnClose() const;

  void print(std::string_view message, std::string_view color = {});
  void printLn(std::string_view message, std::string_view color = {});

  Highlighter highlighter;

private:
  void rebuildText();
  void executeSnowlang();
  void trimHistoryToFit();
  void commitLine();

private:
  WindowManager::WindowID targetWindow;

  std::unordered_map<std::string, std::string> aliases;

  Snowlang::SnowTermIO snowlangIO;
  Snowlang::SnowlangInstance snowlang;

  GameText *text;

  std::deque<std::string> history;
  std::vector<std::string> inputHistory;
  std::string input;
  std::string currentLine;

  static std::vector<Terminal *> s_activeTerminals;

  bool opened = true;
  bool destroyWindowOnClose = false;

  size_t cursorPos = 0;

  int historyBrowseIndex = -1;
  std::string savedInput;
};