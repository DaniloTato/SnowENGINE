#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "GameWindow.hpp"

class WindowManager {
public:
  using WindowID = size_t;
  enum class Set : std::uint8_t { MAIN, TERMINAL, DEVUI };

  struct WindowEntry {
    Set set;
    GameWindow window;
  };

private:
  std::unordered_map<WindowID, WindowEntry> windows;
  WindowID nextId = 0;

  [[nodiscard]] GameWindow *fetchGameWindow(WindowID id);
  [[nodiscard]] const GameWindow *fetchGameWindow(WindowID id) const;

public:
  static WindowManager &getInstance();

  WindowID create(Set set, unsigned int w, unsigned int h,
                  const std::string &name);

  void destroy(WindowID id);

  [[nodiscard]] const std::unordered_map<WindowID, WindowEntry> &getAll() const;

  std::vector<WindowID> getByDomain(Set set);
  [[nodiscard]] Set getDomainOfWindow(WindowID id) const;

  void drawOnWindow(WindowID id, const sf::Sprite &toDraw);
  void drawOnWindow(WindowID id, const sf::RectangleShape &toDraw);
  void drawOnWindow(WindowID id, const sf::CircleShape &toDraw);
  void drawOnWindow(WindowID id, const sf::Text &toDraw);
  void drawOnWindow(WindowID id, const sf::Vertex *vertices, std::size_t count,
                    sf::PrimitiveType type);

  void clearWindow(WindowID id, sf::Color backgroundColor = sf::Color::Black);

  void pauseWindow(WindowID id);
  void resumeWindow(WindowID id);
  [[nodiscard]] bool isWindowPaused(WindowID id);

  void checkRenderFlag(WindowID id);

  void setFrameRate(WindowID id, float value);
  [[nodiscard]] float getFrameRate(WindowID id) const;

  WindowID getMain();
  [[nodiscard]] bool isMainWindowAlive() const;

  bool pollEventOnWindow(WindowID id, sf::Event &event);
};
