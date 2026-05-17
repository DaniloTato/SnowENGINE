#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

#include "GameWindow.hpp"

class IEventListener;

struct WindowID {
  static constexpr std::size_t NullValue =
      std::numeric_limits<std::size_t>::max();
  std::size_t value = NullValue;

  WindowID() = default;
  WindowID(size_t x) { value = x; }

  [[nodiscard]] bool isNull() const { return value == NullValue; }

  explicit operator bool() const { return !isNull(); }

  auto operator<=>(const WindowID &) const = default;
};

namespace std {

template <> struct hash<WindowID> {
  std::size_t operator()(const WindowID &id) const noexcept {
    return std::hash<std::size_t>{}(id.value);
  }
};

} // namespace std

class WindowManager {
public:
  enum class Set : std::uint8_t { MAIN, TERMINAL, DEVUI };

  struct WindowEntry {
    Set set;
    GameWindow window;
  };

private:
  std::unordered_map<WindowID, WindowEntry> windows;
  size_t nextId = 0;

  [[nodiscard]] GameWindow *fetchGameWindow(WindowID id);
  [[nodiscard]] const GameWindow *fetchGameWindow(WindowID id) const;
  void destroy(WindowID id);

  std::unordered_map<WindowID, std::vector<IEventListener *>> listeners;
  std::vector<WindowID> pendingDestroy;

public:
  static WindowManager &getInstance();

  WindowID create(Set set, unsigned int w, unsigned int h,
                  const std::string &name);

  void queueDestroy(WindowID id);

  void applyDestroyQueue();

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

  void subscribe(WindowID id, IEventListener *listener);
  void unsubscribe(WindowID id, IEventListener *listener);
  void pollEvents();
};