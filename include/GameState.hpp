#pragma once

#include "Bullet.hpp"
#include "GameCamera.hpp"
#include "GameObjectExposure.hpp"
#include "GeneralContext.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "WindowTypes.hpp"
#include <chrono>
#include <vector>

enum class CameraTypes : std::uint8_t { MAIN, UI, TERMINAL, COUNT };

struct WindowCreationRequest {
  WindowTypes type;
  int width;
  int height;
  std::string name;
};

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

class GameState {
public:
  static GameState &getInstance();

  void createCamera(CameraTypes type, GameObject::UpdateDomain updateDomain);

  void removeWindow(WindowTypes type);
  void removeWindow(sf::RenderWindow *reference);
  void createWindow(WindowTypes type, int width, int height,
                    const std::string &name);

  GameWindow &getWindowByType(WindowTypes type);

  [[nodiscard]] const std::vector<GameWindow> &getWindows() const;
  [[nodiscard]] std::vector<GameWindow> &getWindows();

  [[nodiscard]] const std::vector<GameCamera *> &getActiveCameras() const;
  [[nodiscard]] GameCamera *getMainCamera() const;
  [[nodiscard]] GameCamera *getUiCamera() const;
  [[nodiscard]] GameCamera *getTerminalCamera() const;
  [[nodiscard]] const sf::RenderWindow *getMainWindow() const;
  [[nodiscard]] sf::RenderWindow *getMainWindow();
  [[nodiscard]] const sf::RenderWindow *getTerminalWindow() const;
  [[nodiscard]] sf::RenderWindow *getTerminalWindow();

  WindowTypes windowPtrToType(const sf::RenderWindow *ptr) const;

  void clearCameras();
  float dt();
  void updateDt();

  void setCheckpoint(sf::Vector2f position);
  [[nodiscard]] const sf::Vector2f getCheckpoint() const;

  [[nodiscard]] int getCrystalAmount() const;
  void addToCrystalAmount(int amount);

  [[nodiscard]] int getPlayerHealth() const;
  void changePlayerHealth(int amount);

  void changeWeaponSelection();
  [[nodiscard]] Bullet::Type getWeaponSelection() const;
  [[nodiscard]] bool hasCheckpoint() const;

  void updateGeneralContext(GeneralContext &ctx);
  [[nodiscard]] const GeneralContext &getGeneralContext();

  [[nodiscard]] const GameObjectExposure::Descriptor &
  getExposedGameState() const;

  void setPrintingObjectIds(bool value);
  [[nodiscard]] bool getPrintingObjectIds() const;

  void setFrameRate(WindowTypes type, float value);
  [[nodiscard]] float getFrameRate(WindowTypes type) const;

  void pauseWindow(WindowTypes type);
  void resumeWindow(WindowTypes type);
  [[nodiscard]] bool isWindowPaused(WindowTypes type);
  void resetDt();

  GameState(const GameState &) = delete;
  GameState &operator=(const GameState &) = delete;

  GameState(GameState &&) = delete;
  GameState &operator=(GameState &&) = delete;

private:
  GameState();

  constexpr std::string windowTypeToString(WindowTypes type) {
    using enum WindowTypes;
    switch (type) {
    case MAIN:
      return "main";
    case TERMINAL:
      return "terminal";
    case COUNT:
      return "count";
    }
    return "unknown";
  }

private:
  std::vector<GameCamera *> activeCameras;
  std::vector<GameWindow> activeWindows;

  std::chrono::high_resolution_clock::time_point lastFrameTime =
      std::chrono::high_resolution_clock::now();
  float dtValue;

  GeneralContext generalContext;

  GameObjectExposure::Descriptor exposedGameState;

  sf::Vector2f checkpoint;

  int crystals;
  int playerHealth;
  Bullet::Type selectedWeapon;

  bool printingGameObjectIds = false;
};