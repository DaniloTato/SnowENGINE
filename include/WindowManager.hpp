#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>

#include "GameWindow.hpp"
#include "MapLifecycle.hpp"
#include "WindowID.hpp"

class IEventListener;

class WindowManager {
public:
  enum class Set : std::uint8_t { MAIN, TERMINAL, DEVUI };

  struct WindowEntry {
    Set set;
    GameWindow window;
  };

private:
  std::unordered_map<WindowID, WindowEntry> windows;
  MapLifecycle<WindowID, WindowEntry> lifecycle;
  size_t nextId = 0;

  [[nodiscard]] GameWindow *fetchGameWindow(WindowID id);
  [[nodiscard]] const GameWindow *fetchGameWindow(WindowID id) const;

  std::unordered_map<WindowID, std::vector<IEventListener *>> listeners;

public:
  WindowManager() = default;
  ~WindowManager() = default;
  WindowManager(const WindowManager &) = delete;
  WindowManager &operator=(const WindowManager &) = delete;
  WindowManager(WindowManager &&) = delete;
  WindowManager &operator=(WindowManager &&) = delete;

  WindowID create(Set set, unsigned int w, unsigned int h,
                  const std::string &name);

  void queueDestroy(WindowID id);

  void applyQueues();

  [[nodiscard]] const std::unordered_map<WindowID, WindowEntry> &getAll() const;

  std::vector<WindowID> getByDomain(Set set);
  [[nodiscard]] Set getDomainOfWindow(WindowID id) const;

  void drawOnWindow(WindowID id, const sf::Drawable &toDraw);
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