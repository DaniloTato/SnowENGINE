#pragma once
#include "GameObjectExposure.hpp"
#include "SFML/System/Vector2.hpp"
#include "WindowManager.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

struct GeneralContext;

class GameObject {
public:
  struct UpdateDomain {
    std::vector<WindowID> windows;
    std::vector<WindowManager::Set> domains;

    bool matches(WindowManager &wm, WindowID id) const;

    UpdateDomain(WindowID id) : windows({id}) {}

    UpdateDomain(std::vector<WindowID> &&ids) : windows(std::move(ids)) {}

    UpdateDomain(WindowManager::Set domain) : domains({domain}) {}

    UpdateDomain(std::vector<WindowManager::Set> &&doms)
        : domains(std::move(doms)) {}
  };

  GameObject(UpdateDomain updateDomain, sf::Vector2f pos = {0.f, 0.f});
  virtual ~GameObject();
  virtual void update(const GeneralContext &ctx) = 0;

  void makePersistentAcrossScenes();
  [[nodiscard]] bool isPersistentAcrossScenes() const;

  virtual std::shared_ptr<GameObjectExposure::Descriptor> describe();
  unsigned int getId();
  bool isUpdateDomainPaused();

  static std::vector<GameObject *> &getGameObjects();
  static void destroy(GameObject *g);

  static void destroySceneObjects();

  static GameObject *findGameObjectById(float id);

  sf::Vector2f position;
  sf::Vector2f offset;

  GameObject(const GameObject &) = delete;
  GameObject &operator=(const GameObject &) = delete;

protected:
  bool persistentAcrossScenes = false;
  static std::vector<GameObject *> s_gameObjects;

private:
  unsigned int id;
  static unsigned int nextId;
  UpdateDomain updateDomain;
};