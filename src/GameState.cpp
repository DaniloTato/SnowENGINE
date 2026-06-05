#include "GameState.hpp"
#include "Bullet.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

#define ENUM_TO_STR =

GameState::GameState()
    : checkpoint({-1, -1}), playerHealth(3),
      selectedWeapon(Bullet::Type::Normal) {}

GameState &GameState::getInstance() {
  static GameState instance;
  return instance;
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

  // exposedGameState.fields["window"] =
  //     GameObjectExposure::makeUnmutableField<GameObjectExposure::Value::Object>(
  //         [this] {
  //           auto windowsObj =
  //               std::make_shared<GameObjectExposure::Descriptor>();

  //           for (size_t i = 0; i < activeWindows.size(); ++i) {
  //             WindowTypes type = static_cast<WindowTypes>(i);

  //             windowsObj->fields[windowTypeToString(type)] =
  //                 GameObjectExposure::makeUnmutableField<
  //                     GameObjectExposure::Value::Object>([this, type] {
  //                   auto index = static_cast<size_t>(type);

  //                   if (index >= activeWindows.size())
  //                     return GameObjectExposure::Value::Object{};

  //                   auto &gw = activeWindows[index];

  //                   if (!gw.getWindow())
  //                     return GameObjectExposure::Value::Object{};

  //                   return gw.describe();
  //                 });
  //           }

  //           return windowsObj;
  //         });

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

void GameState::resetDt() {
  lastFrameTime = std::chrono::high_resolution_clock::now();
  dtValue = 0.f;
}