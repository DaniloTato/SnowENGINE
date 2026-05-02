#include "WindowManager.hpp"
#include "Constants.hpp"

#include <iostream>
#include <stdexcept>

WindowManager &WindowManager::getInstance() {
  static WindowManager instance;
  return instance;
}

WindowManager::WindowID WindowManager::create(Domain domain, int w, int h,
                                              const std::string &name) {

  if (domain == Domain::MAIN) {
    for (const auto &[id, entry] : windows) {
      if (entry.domain == Domain::MAIN) {
        throw std::runtime_error("[WindowManager] MAIN window already exists");
      }
    }
  }

  WindowID id = nextId++;

  GameWindow gw(nullptr, Constants::FRAME_RATE);
  gw.setWindow(new sf::RenderWindow(sf::VideoMode(w, h), name));

  windows.emplace(id, WindowEntry{.domain = domain, .window = gw});

  return id;
}

sf::RenderWindow *WindowManager::get(WindowID id) {
  auto it = windows.find(id);
  if (it == windows.end()) {
    return nullptr;
  }
  return it->second.window.getWindow();
}

const sf::RenderWindow *WindowManager::get(WindowID id) const {
  return fetchGameWindow(id)->getWindow();
}

void WindowManager::destroy(WindowID id) {
  auto it = windows.find(id);
  if (it == windows.end())
    return;

  auto *win = it->second.window.getWindow();

  if (win && win->isOpen()) {
    win->close();
  }

  delete win;
  windows.erase(it);
}

const std::unordered_map<WindowManager::WindowID, WindowManager::WindowEntry> &
WindowManager::getAll() const {
  return windows;
}

std::vector<WindowManager::WindowID> WindowManager::getByDomain(Domain domain) {

  std::vector<WindowID> result;

  for (const auto &[id, entry] : windows) {
    if (entry.domain == domain) {
      result.push_back(id);
    }
  }

  return result;
}

WindowManager::WindowID WindowManager::getMain() {
  for (const auto &[id, entry] : windows) {
    if (entry.domain == Domain::MAIN) {
      return id;
    }
  }

  throw std::runtime_error("[WindowManager] No MAIN window found");
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

void WindowManager::pauseWindow(WindowManager::WindowID id) {
  GameWindow *ptr = fetchGameWindow(id);
  if (!ptr) {
    std::cerr << "[WindowManager] tried to pause a non-fetchable window\n";
    return;
  }

  ptr->pause();
}

void WindowManager::resumeWindow(WindowManager::WindowID id) {
  GameWindow *ptr = fetchGameWindow(id);
  if (!ptr) {
    std::cerr << "[WindowManager] tried to resume a non-fetchable window\n";
    return;
  }
  ptr->resume();
}

bool WindowManager::isWindowPaused(WindowManager::WindowID id) {
  GameWindow *ptr = fetchGameWindow(id);
  if (!ptr) {
    std::cerr << "[WindowManager] failed to get pause state from a "
                 "non-fetchable window\n";
    return false;
  }
  return ptr->isPaused();
}

WindowManager::Domain WindowManager::getDomainOfWindow(WindowID id) const {
  auto it = windows.find(id);
  if (it == windows.end()) {
    std::cerr << "[WindowManager] failed to get domain of non-fetchable "
                 "window. Defaulting to MAIN\n";
    return Domain::MAIN;
  }
  return it->second.domain;
}

void WindowManager::setFrameRate(WindowID id, float value) {
  GameWindow *window = fetchGameWindow(id);
  window->updateFrameRate(value);
}

float WindowManager::getFrameRate(WindowID id) const {
  const GameWindow *window = fetchGameWindow(id);
  return window->getFrameRate();
}