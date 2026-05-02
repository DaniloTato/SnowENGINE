#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "GameWindow.hpp"

class WindowManager {
public:
  using WindowID = size_t;
  enum class Domain : std::uint8_t { MAIN, TERMINAL, DEVUI };

private:
  struct WindowEntry {
    Domain domain;
    GameWindow window;
  };

  std::unordered_map<WindowID, WindowEntry> windows;
  WindowID nextId = 0;

  [[nodiscard]] GameWindow *fetchGameWindow(WindowID id);
  [[nodiscard]] const GameWindow *fetchGameWindow(WindowID id) const;

public:
  static WindowManager &getInstance();

  WindowID create(Domain domain, int w, int h, const std::string &name);

  sf::RenderWindow *get(WindowID id);
  [[nodiscard]] const sf::RenderWindow *get(WindowID id) const;
  void destroy(WindowID id);

  [[nodiscard]] const std::unordered_map<WindowID, WindowEntry> &getAll() const;

  std::vector<WindowID> getByDomain(Domain domain);
  [[nodiscard]] Domain getDomainOfWindow(WindowID id) const;

  void pauseWindow(WindowID id);
  void resumeWindow(WindowID id);
  [[nodiscard]] bool isWindowPaused(WindowID id);

  void setFrameRate(WindowID id, float value);
  [[nodiscard]] float getFrameRate(WindowID id) const;

  WindowID getMain();
};
