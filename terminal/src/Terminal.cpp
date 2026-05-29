#include "Terminal.hpp"

#include "Constants.hpp"
#include "GameState.hpp"
#include "GameText.hpp"

#include "SnowlangInstance.hpp"

#include <SFML/Graphics.hpp>
#include <ranges>
#include <string_view>

static constexpr size_t MAX_LINES = 37;

std::vector<Terminal *> Terminal::s_activeTerminals;

size_t Terminal::countVisualLines(const TerminalLine &line) {

  size_t lines = 1;

  for (char c : line.text) {
    if (c == '\n') {
      lines++;
    }
  }

  // Only markup lines interpret [ln]
  if (line.type == LineType::Markup) {

    constexpr std::string_view token = "[ln]";

    size_t pos = 0;

    while ((pos = line.text.find(token, pos)) != std::string::npos) {
      lines++;
      pos += token.size();
    }
  }

  return lines;
}

Terminal::Terminal(WindowID window, CameraID camera, CameraID tileCamera,
                   Engine &engine, sf::Texture *fontTexture)
    : targetWindow(window), snowlangIO(*this), snowlang(snowlangIO),
      text(new GameText({.engine = engine,
                         .window = targetWindow,
                         .texture = fontTexture,
                         .camera = camera,
                         .layer = Constants::UI_LAYER,
                         .parallax = 1.f})) {

  // QUICK FIX FOR NOW, CHANGE LATER.
  snowlang.tileCamera = nullptr;

  s_activeTerminals.push_back(this);
  text->position = {10.f, 10.f};
  text->makePersistentAcrossScenes();

  history.emplace_back(TerminalLine{
      .text =
          R"([color=cyan][anim=sin]SNOWLANG[/anim] Developer Console[/color])",
      .type = LineType::Markup});

  history.emplace_back(TerminalLine{
      .text =
          R"(Type [color=yellow]print_commands[/color] for command listing[ln])",
      .type = LineType::Markup});

  rebuildText();
}

Terminal::~Terminal() { GameObject::destroy(text); }

std::string Terminal::escapeInput(const std::string &s) {
  std::string out;

  for (char c : s) {
    if (c == '[' || c == ']') {
      out += '\\';
    }

    out += c;
  }

  return out;
}

void Terminal::update() { snowlang.update(GameState::getInstance().dt()); }

void Terminal::handleEvent(WindowID windowId, const sf::Event &event) {

  if (event.type == sf::Event::Closed) {
    close();
    return;
  }

  // TEXT INPUT (insert at cursor)
  if (event.type == sf::Event::TextEntered) {
    if (event.text.unicode >= 32 && event.text.unicode < 127 &&
        event.text.unicode != '\\') {

      char c = static_cast<char>(event.text.unicode);

      input.insert(input.begin() +
                       static_cast<std::string::difference_type>(cursorPos),
                   c);

      cursorPos++;

      rebuildText();
    }
  }

  if (event.type == sf::Event::KeyPressed) {

    if (event.key.code == sf::Keyboard::Escape) {
      kill();
      return;
    }

    if (event.key.code == sf::Keyboard::Backspace) {
      if (cursorPos > 0) {
        input.erase(input.begin() +
                    static_cast<std::string::difference_type>(cursorPos) - 1);
        cursorPos--;
        rebuildText();
      }
      return;
    }

    if (event.key.code == sf::Keyboard::Left) {
      if (cursorPos > 0)
        cursorPos--;
      rebuildText();
      return;
    }

    if (event.key.code == sf::Keyboard::Right) {
      if (cursorPos < input.size())
        cursorPos++;
      rebuildText();
      return;
    }

    if (event.key.code == sf::Keyboard::Up) {
      if (inputHistory.empty())
        return;

      if (historyBrowseIndex == -1) {
        savedInput = input;
        historyBrowseIndex = static_cast<int>(inputHistory.size()) - 1;
      } else if (historyBrowseIndex > 0) {
        historyBrowseIndex--;
      }

      input = inputHistory[historyBrowseIndex];
      cursorPos = input.size();
      rebuildText();
      return;
    }

    if (event.key.code == sf::Keyboard::Down) {
      if (historyBrowseIndex == -1)
        return;

      historyBrowseIndex++;

      if (historyBrowseIndex >= static_cast<int>(inputHistory.size())) {
        historyBrowseIndex = -1;
        input = savedInput;
      } else {
        input = inputHistory[historyBrowseIndex];
      }

      cursorPos = input.size();
      rebuildText();
      return;
    }

    if (event.key.code == sf::Keyboard::Enter) {
      if (inputHistory.empty() || inputHistory.back() != input) {
        inputHistory.push_back(input);
      }

      history.emplace_back(
          TerminalLine{.text = "> " + input, .type = LineType::Raw});

      snowlangIO.pushLine(input);
      executeSnowlang();

      input.clear();
      input.clear();
      cursorPos = 0;
      historyBrowseIndex = -1;

      while (history.size() > MAX_LINES) {
        history.pop_front();
      }

      rebuildText();
      return;
    }
  }
}

void Terminal::kill() { opened = false; }

void Terminal::close() {
  kill();
  destroyWindowOnClose = true;
}

void Terminal::print(std::string_view message, std::string_view color) {
  if (!color.empty()) {
    currentLine += "[color=";
    currentLine += color;
    currentLine += "]";
    currentLine += message;
    currentLine += "[/color]";
  } else {
    currentLine += message;
  }

  rebuildText();
}

void Terminal::printLn(std::string_view message, std::string_view color) {
  print(message, color);
  commitLine();
}

void Terminal::commitLine() {
  history.emplace_back(
      TerminalLine{.text = currentLine, .type = LineType::Markup});
  currentLine.clear();
  rebuildText();
}

bool Terminal::isOpen() const { return opened; }

void Terminal::rebuildText() {
  trimHistoryToFit();

  std::string markup;
  markup += "#position 10 10\n";
  markup += "#boundary 880\n";

  for (const auto &line : history) {

    if (line.type == LineType::Markup) {
      markup += line.text;
    } else {
      markup += escapeInput(line.text);
    }

    markup += "\n";
  }

  if (!currentLine.empty()) {
    markup += currentLine;
  }

  std::string visibleInput = input;
  visibleInput.insert(cursorPos, "|");
  visibleInput = escapeInput(visibleInput);

  visibleInput = highlighter.applyParenHighlight(visibleInput);

  markup += "[color=yellow]> " + visibleInput + "[/color]";

  if (text) {
    text->loadFromMarkup(markup);
  }
}

void Terminal::executeSnowlang() {
  if (snowlang.io.hasLine()) {
    snowlang.run(snowlang.io.readLine());
  }
}

WindowID Terminal::getTargetWindow() const { return targetWindow; }

bool Terminal::destroysWindowOnClose() const { return destroyWindowOnClose; }

void Terminal::clear() { history.clear(); }

void Terminal::destroyKilledTerminals(WindowManager &wm) {
  auto isKilled = [&wm](Terminal *t) {
    if (!t->isOpen()) {
      if (t->destroysWindowOnClose()) {
        wm.queueDestroy(t->getTargetWindow());
      }
      delete t;
      return true;
    }
    return false;
  };

  auto newEnd = std::ranges::remove_if(s_activeTerminals, isKilled);
  s_activeTerminals.erase(newEnd.begin(), s_activeTerminals.end());
}

void Terminal::trimHistoryToFit() {
  size_t totalLines = 0;

  for (const auto &line : history | std::views::reverse) {
    totalLines += countVisualLines(line);
  }

  while (!history.empty() && totalLines > MAX_LINES) {
    totalLines -= countVisualLines(history.front());
    history.pop_front();
  }
}