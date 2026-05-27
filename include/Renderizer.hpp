#pragma once
#include "GameCamera.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include <SFML/Graphics.hpp>

struct RenderizerParameters;

struct RenderEntry {
  GameObject *object;
  class Renderizer *renderizer;
  bool isRectShape = false;
};

class Renderizer {
public:
  Renderizer(const RenderizerParameters &params);
  virtual ~Renderizer();

  void setRect(const sf::IntRect &newRect, int direction);
  const sf::IntRect &getRect() const;
  virtual void render(GameObject *ob);
  void renderRectShape(GameObject *obj);
  float getLayer() const;
  [[nodiscard]] const GameCamera *getAssignedCamera() const;
  [[nodiscard]] const sf::Sprite &getSprite() const;
  void setLayer(float newLayer);
  void setColor(sf::Color newColor);
  void toggleShowEvery(float time);
  void toggleColorEvery(float time, const sf::Color &color1,
                        const sf::Color &color2);
  bool shouldIRender();
  void turnOffCulling();

  WindowID getWindow() const;

  void hide();

  bool isVisible() const;

  static void registerPair(GameObject *obj, Renderizer *rend, bool isRectShape);
  static void unregisterPair(Renderizer *rend);
  static void unregisterByWindow(WindowID window);

  static void renderAll();
  void showSprite();

protected:
  WindowID window;
  sf::Sprite sprite;
  sf::Texture &texture;
  sf::IntRect rect;
  sf::Color color;
  GameCamera *assignedCamera;
  WindowManager &windowManager;
  float layer;
  float paralax;

  bool show;
  float showCountDown;
  float colorCountDown;

  bool hasCulling;

  static std::vector<RenderEntry> registry;
};