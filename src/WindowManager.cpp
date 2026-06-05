#include "WindowManager.hpp"
#include "Constants.hpp"
#include "IEventListener.hpp"

#include <iostream>
#include <stdexcept>

WindowID WindowManager::create(Set set, unsigned int w, unsigned int h,
                               const std::string &name) {
  if (set == Set::MAIN) {
    for (const auto &[id, entry] : windows) {
      if (entry.set == Set::MAIN) {
        throw std::runtime_error("[WindowManager] MAIN window already exists");
      }
    }
  }

  WindowID id(nextId++);

  GameWindow gw(nullptr, Constants::FRAME_RATE);
  gw.setWindow(new sf::RenderWindow(sf::VideoMode(w, h), name));

  lifecycle.queueCreate(id, WindowEntry{.set = set, .window = gw});
  applyQueues();

  return id;
}

void WindowManager::queueDestroy(WindowID id) { lifecycle.queueDestroy(id); }

void WindowManager::applyQueues() { lifecycle.apply(windows); }

const std::unordered_map<WindowID, WindowManager::WindowEntry> &
WindowManager::getAll() const {
  return windows;
}

// Right now, inefficient. It allocates a vector each frame.
// Better approaach would be to update and cache the sets only when a window is
// created
std::vector<WindowID> WindowManager::getByDomain(Set set) {

  std::vector<WindowID> result;

  for (const auto &[id, entry] : windows) {
    if (entry.set == set) {
      result.push_back(id);
    }
  }

  return result;
}

WindowID WindowManager::getMain() {
  for (const auto &[id, entry] : windows) {
    if (entry.set == Set::MAIN) {
      return id;
    }
  }

  throw std::runtime_error("[WindowManager] No MAIN window found");
}

// Very inefficient hahahah.
bool WindowManager::isMainWindowAlive() const {
  for (const auto &[id, entry] : windows) {
    if (entry.set == Set::MAIN) {
      return true;
    }
  }

  return false;
}

GameWindow *WindowManager::fetchGameWindow(WindowID id) {
  auto it = windows.find(id);
  if (it == windows.end()) {
    return nullptr;
  }
  return &it->second.window;
}

const GameWindow *WindowManager::fetchGameWindow(WindowID id) const {
  auto it = windows.find(id);
  if (it == windows.end()) {
    return nullptr;
  }
  return &it->second.window;
}

void WindowManager::pauseWindow(WindowID id) {
  GameWindow *ptr = fetchGameWindow(id);
  if (!ptr) {
    std::cerr << "[WindowManager] tried to pause a non-fetchable window\n";
    return;
  }

  ptr->pause();
}

void WindowManager::resumeWindow(WindowID id) {
  GameWindow *ptr = fetchGameWindow(id);
  if (!ptr) {
    std::cerr << "[WindowManager] tried to resume a non-fetchable window\n";
    return;
  }
  ptr->resume();
}

bool WindowManager::isWindowPaused(WindowID id) {
  GameWindow *ptr = fetchGameWindow(id);
  if (!ptr) {
    std::cerr << "[WindowManager] failed to get pause state from a "
                 "non-fetchable window\n";
    return true;
  }
  return ptr->isPaused();
}

WindowManager::Set WindowManager::getDomainOfWindow(WindowID id) const {
  auto it = windows.find(id);
  if (it == windows.end()) {
    std::cerr << "[WindowManager] failed to get domain of non-fetchable "
                 "window. Defaulting to MAIN\n";
    return Set::MAIN;
  }
  return it->second.set;
}

void WindowManager::setFrameRate(WindowID id, float value) {
  GameWindow *window = fetchGameWindow(id);
  window->updateFrameRate(value);
}

float WindowManager::getFrameRate(WindowID id) const {
  const GameWindow *window = fetchGameWindow(id);
  return window->getFrameRate();
}

void WindowManager::checkRenderFlag(WindowID id) {
  GameWindow *window = fetchGameWindow(id);
  if (window->renderFlag()) {
    window->getWindow()->display();
    window->markAsRendered();
  }
}

void WindowManager::drawOnWindow(WindowID id, const sf::Drawable &drawable) {
  if (id.isNull()) {
    std::cout << "[WindowManager] [drawOnWindow] tried to draw over window "
                 "with NULL ID.\n";
  }
  auto *gw = fetchGameWindow(id);
  if (!gw) {
    std::cout << "[WindowManager] [drawOnWindow] valid id fetched nullptr of "
                 "GameWindow\n";
    return;
  }
  auto *window = gw->getWindow();
  if (!window) {
    std::cout << "[WindowManager] [drawOnWindow] valid gameWindow holds null "
                 "(potentially destroyed) "
                 "WindowInstance\n";
    return;
  }
  window->draw(drawable);
}

void WindowManager::drawOnWindow(WindowID id, const sf::Vertex *vertices,
                                 std::size_t count, sf::PrimitiveType type) {
  if (id.isNull()) {
    std::cout
        << "[WindowManager] [drawOnWindow(lines)] tried to draw over window "
           "with NULL ID.\n";
  }
  auto *gw = fetchGameWindow(id);
  if (!gw) {
    std::cout << "[WindowManager] [drawOnWindow(lines)] valid id fetched "
                 "nullptr of GameWindow\n";
    return;
  }
  auto *window = gw->getWindow();
  if (!window) {
    std::cout
        << "[WindowManager] [drawOnWindow(lines)] valid gameWindow holds null "
           "(potentially destroyed) "
           "WindowInstance\n";
    return;
  }
  window->draw(vertices, count, type);
}

void WindowManager::clearWindow(WindowID id, sf::Color backgroundColor) {
  fetchGameWindow(id)->getWindow()->clear(backgroundColor);
}

void WindowManager::subscribe(WindowID id, IEventListener *listener) {

  if (id.isNull()) {
    throw std::runtime_error(
        "[WindowManager] [subscribe] Subscribed listener to null ID");
  }

  listeners[id].push_back(listener);
}

void WindowManager::unsubscribe(WindowID id, IEventListener *listener) {

  if (id.isNull()) {
    throw std::runtime_error(
        "[WindowManager] [unsubscribe] Unsubscribed listener to null ID");
  }

  auto it = listeners.find(id);
  if (it == listeners.end()) {
    return;
  }

  auto &vec = it->second;
  auto [first, last] = std::ranges::remove(vec, listener);
  vec.erase(first, last);
}

void WindowManager::pollEvents() {
  sf::Event event;
  for (auto &[id, entry] : windows) {
    auto *win = entry.window.getWindow();
    while (win->pollEvent(event)) {
      for (auto *listener : listeners[id]) {
        listener->handleEvent(id, event);
      }
    }
  }
}
