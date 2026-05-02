#include "GameState.hpp"
#include "Bullet.hpp"
#include "Constants.hpp"
#include "GameCamera.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

#define ENUM_TO_STR =

GameState::GameState()
    : activeCameras({}),
      activeWindows(static_cast<size_t>(WindowTypes::COUNT),
                    GameWindow(nullptr, Constants::FRAME_RATE)),
      checkpoint({-1, -1}), playerHealth(3),
      selectedWeapon(Bullet::Type::Normal) {

  activeWindows[static_cast<size_t>(WindowTypes::MAIN)].setWindow(
      new sf::RenderWindow(
          sf::VideoMode(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT),
          Constants::MAIN_WINDOW_NAME));
}

GameState &GameState::getInstance() {
  static GameState instance;
  return instance;
}

void GameState::removeWindow(WindowTypes type) {
  auto index = static_cast<size_t>(type);
  if (activeWindows.size() <= index)
    return;

  auto &gw = activeWindows[index];
  if (!gw.getWindow())
    return;

  if (gw.getWindow()->isOpen()) {
    gw.getWindow()->close();
  }

  Renderizer::unregisterByWindow(gw.getWindow());
  delete gw.getWindow();
  gw.setWindow(nullptr);
}

void GameState::removeWindow(sf::RenderWindow *reference) {
  for (size_t i = 0; i < activeWindows.size(); ++i) {
    if (activeWindows[i].getWindow() == reference) {
      removeWindow(static_cast<WindowTypes>(i));
      return;
    }
  }
}

const std::vector<GameCamera *> &GameState::getActiveCameras() const {
  return activeCameras;
}

GameCamera *GameState::getMainCamera() const {
  size_t mainCameraIndex = static_cast<int>(CameraTypes::MAIN);
  if (activeCameras.size() >= mainCameraIndex) {
    return activeCameras[mainCameraIndex];
  }
  throw std::runtime_error("[GameState] Main camera not yet initilized\n");
  return nullptr;
}

const sf::RenderWindow *GameState::getMainWindow() const {
  if (activeWindows.empty()) {
    throw std::runtime_error("[GameState] Main window not yet initilized\n");
    return nullptr;
  }
  return activeWindows.front().getWindow();
}

sf::RenderWindow *GameState::getMainWindow() {
  if (activeWindows.empty()) {
    throw std::runtime_error("[GameState] Main window not yet initilized\n");
    return nullptr;
  }
  return activeWindows.front().getWindow();
}

const sf::RenderWindow *GameState::getTerminalWindow() const {
  size_t index = static_cast<size_t>(WindowTypes::TERMINAL);
  if (activeWindows.size() > index) {
    return activeWindows[index].getWindow();
  }
  throw std::runtime_error(
      "[GameState] [getTerminalWindow()] Terminal window not yet initilized\n");
  return nullptr;
}

sf::RenderWindow *GameState::getTerminalWindow() {
  size_t index = static_cast<size_t>(WindowTypes::TERMINAL);
  if (activeWindows.size() > index) {
    return activeWindows[index].getWindow();
  }
  throw std::runtime_error(
      "[GameState] [getTerminalWindow()] Terminal window not yet initilized\n");
  return nullptr;
}

void GameState::clearCameras() {
  for (size_t i = 0; i < activeCameras.size(); i++) {
    if (activeCameras[i] &&
        static_cast<CameraTypes>(i) != CameraTypes::TERMINAL &&
        !activeCameras[i]->isPersistentAcrossScenes()) {
      GameObject::destroy(activeCameras[i]);
    }
  }
  activeCameras.clear();
}

float GameState::dt() { return dtValue; }

void GameState::updateDt() {
  auto now = std::chrono::high_resolution_clock::now();
  float elapsed = std::chrono::duration<float>(now - lastFrameTime).count();
  lastFrameTime = now;

  constexpr float MAX_DT = 1.f / 20.f;
  if (elapsed > MAX_DT)
    elapsed = MAX_DT;

  dtValue = elapsed;
}

GameCamera *GameState::getUiCamera() const {
  size_t uiCameraIndex = static_cast<int>(CameraTypes::UI);
  if (activeCameras.size() >= uiCameraIndex) {
    return activeCameras[uiCameraIndex];
  }
  throw std::runtime_error(
      "[GameState] [getUiCamera()] UI camera not yet initilized\n");
  return nullptr;
}

GameCamera *GameState::getTerminalCamera() const {
  size_t consoleCameraIndex = static_cast<int>(CameraTypes::TERMINAL);
  if (activeCameras.size() >= consoleCameraIndex) {
    return activeCameras[consoleCameraIndex];
  }
  throw std::runtime_error(
      "[GameState] [getTerminalCamera()] Terminal camera not yet initilized\n");
  return nullptr;
}

void GameState::createCamera(CameraTypes type,
                             GameObject::UpdateDomain updateDomain) {

  auto index = static_cast<size_t>(type);

  if (activeCameras.size() < static_cast<int>(CameraTypes::COUNT)) {
    activeCameras.resize(static_cast<int>(CameraTypes::COUNT));
  }

  if (activeCameras[index]) {
    return;
  }

  activeCameras[index] = new GameCamera(std::move(updateDomain));

  if (type == CameraTypes::TERMINAL) {
    activeCameras[index]->makePersistentAcrossScenes();
  }
}

void GameState::createWindow(WindowTypes type, int width, int height,
                             const std::string &name) {
  auto index = static_cast<size_t>(type);

  if (activeWindows.size() < static_cast<size_t>(WindowTypes::COUNT)) {
    activeWindows.resize(static_cast<size_t>(WindowTypes::COUNT),
                         GameWindow(nullptr, Constants::FRAME_RATE));
  }

  auto &gw = activeWindows[index];
  if (gw.getWindow())
    return;

  gw.setWindow(new sf::RenderWindow(sf::VideoMode(width, height), name));
}

int GameState::getCrystalAmount() const { return crystals; }

void GameState::addToCrystalAmount(int amount) { crystals += amount; }

int GameState::getPlayerHealth() const { return playerHealth; }

void GameState::changePlayerHealth(int amount) { playerHealth += amount; }

void GameState::changeWeaponSelection() {
  if (selectedWeapon == Bullet::Type::Normal) {
    selectedWeapon = Bullet::Type::BubbleGun;
  } else {
    selectedWeapon = Bullet::Type::Normal;
  }
}

Bullet::Type GameState::getWeaponSelection() const { return selectedWeapon; }

void GameState::setCheckpoint(sf::Vector2f position) { checkpoint = position; }

const sf::Vector2f GameState::getCheckpoint() const { return checkpoint; }

bool GameState::hasCheckpoint() const {
  return checkpoint != sf::Vector2f{-1, -1};
}

const std::vector<GameWindow> &GameState::getWindows() const {
  return activeWindows;
}

[[nodiscard]] std::vector<GameWindow> &GameState::getWindows() {
  return activeWindows;
}

GameWindow &GameState::getWindowByType(WindowTypes type) {
  auto index = static_cast<size_t>(type);
  return activeWindows[index];
}

void GameState::updateGeneralContext(GeneralContext &ctx) {
  generalContext = ctx;

  exposedGameState.fields["player"] = GameObjectExposure::makeUnmutableField<
      GameObjectExposure::Value::Object>([this] {
    if (!generalContext.player) {
      std::cout
          << "[GameState] Warning: describing player, but player is nullptr\n";
      return std::make_shared<GameObjectExposure::Descriptor>();
    }
    return generalContext.player->describe();
  });

  exposedGameState.fields["hp"] =
      GameObjectExposure::makePublicField(playerHealth);

  exposedGameState.fields["crystals"] =
      GameObjectExposure::makePublicField(crystals);

  exposedGameState.fields["camera"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [] {
            return GameObjectExposure::Descriptor::describeActiveCameraList();
          });

  exposedGameState.fields["window"] =
      GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
          [this] {
            auto windowsObj =
                std::make_shared<GameObjectExposure::Descriptor>();

            for (size_t i = 0; i < activeWindows.size(); ++i) {
              WindowTypes type = static_cast<WindowTypes>(i);

              windowsObj->fields[windowTypeToString(type)] =
                  GameObjectExposure::makeUnmutableField<
                      GameObjectExposure::Value::Object>([this, type] {
                    auto index = static_cast<size_t>(type);

                    if (index >= activeWindows.size())
                      return GameObjectExposure::Value::Object{};

                    auto &gw = activeWindows[index];

                    if (!gw.getWindow())
                      return GameObjectExposure::Value::Object{};

                    return gw.describe();
                  });
            }

            return windowsObj;
          });

  exposedGameState.fields["print_id"] = GameObjectExposure::makeField<bool>(
      [] { return GameState::getInstance().getPrintingObjectIds(); },
      [](bool v) { GameState::getInstance().setPrintingObjectIds(v); });
}

const GameObjectExposure::Descriptor &GameState::getExposedGameState() const {
  return exposedGameState;
}

const GeneralContext &GameState::getGeneralContext() { return generalContext; }

void GameState::setPrintingObjectIds(bool value) {
  printingGameObjectIds = value;
}

bool GameState::getPrintingObjectIds() const { return printingGameObjectIds; }

void GameState::setFrameRate(WindowTypes type, float value) {
  auto index = static_cast<size_t>(type);
  if (activeWindows.size() > index) {
    if (activeWindows[index].getWindow()) {
      activeWindows[index].updateFrameRate(value);
    }
  }
}

float GameState::getFrameRate(WindowTypes type) const {
  auto index = static_cast<size_t>(type);
  if (activeWindows.size() > index) {
    return activeWindows[index].getFrameRate();
  }
  return 0;
}

void GameState::pauseWindow(WindowTypes type) {
  getWindowByType(type).pause();
  GameState::getInstance().resetDt();
}

void GameState::resumeWindow(WindowTypes type) {
  getWindowByType(type).resume();
  GameState::getInstance().resetDt();
}

bool GameState::isWindowPaused(WindowTypes type) {
  return getWindowByType(type).isPaused();
}

void GameState::resetDt() {
  lastFrameTime = std::chrono::high_resolution_clock::now();
  dtValue = 0.f;
}

WindowTypes GameState::windowPtrToType(const sf::RenderWindow *ptr) const {
  if (!ptr)
    return WindowTypes::COUNT;

  for (size_t i = 0; i < activeWindows.size(); ++i) {
    if (activeWindows[i].getWindow() == ptr) {
      return static_cast<WindowTypes>(i);
    }
  }

  return WindowTypes::COUNT;
}