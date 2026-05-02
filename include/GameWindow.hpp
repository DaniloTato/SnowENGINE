#include "GameObjectExposure.hpp"

struct GameWindow {

  [[nodiscard]] GameObjectExposure::Value::Object describe() {
    auto desc = std::make_shared<GameObjectExposure::Descriptor>();

    desc->fields["fps"] = GameObjectExposure::makeField<float>(
        [this] { return getFrameRate(); },
        [this](float v) { updateFrameRate(v); });

    desc->fields["paused"] = GameObjectExposure::makePublicField<bool>(paused);

    return desc;
  }

  GameWindow() = default;

  GameWindow(sf::RenderWindow *window, float frameRate)
      : window(window), frameRate(frameRate) {
    if (window) {
      window->setFramerateLimit(0); // Run as fast as it can
      window->setVerticalSyncEnabled(false);
    }
  }

  bool renderFlag() {
    if (frameRate <= 0)
      return true;
    float target = 1.f / frameRate;
    return clock.getElapsedTime().asSeconds() >= target;
  }

  void markAsRendered() { clock.restart(); }

  void setWindow(sf::RenderWindow *newWindow) {
    window = newWindow;
    updateFrameRate(frameRate);
  }
  [[nodiscard]] sf::RenderWindow *getWindow() { return window; }
  [[nodiscard]] const sf::RenderWindow *getWindow() const { return window; }

  [[nodiscard]] float getFrameRate() const { return frameRate; }

  void updateFrameRate(float newFrameRate) { frameRate = newFrameRate; }

  void pause() { paused = true; }

  void resume() { paused = false; }

  [[nodiscard]] bool isPaused() const { return paused; }

private:
  sf::RenderWindow *window;
  sf::Clock clock;
  float frameRate;
  bool paused = false;
};