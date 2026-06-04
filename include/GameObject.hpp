#pragma once
#include "GameObjectExposure.hpp"
#include "SFML/System/Vector2.hpp"
#include "WindowManager.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

struct GeneralContext;
class SpriteComponent;

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
  virtual ~GameObject() = default;
  virtual void update(const GeneralContext &ctx) = 0;

  virtual std::shared_ptr<GameObjectExposure::Descriptor> describe();
  unsigned int getId();
  bool isUpdateDomainPaused(WindowManager &wm);

  void setUpdateDomain(const UpdateDomain &newDomain);

  void destroyLater();
  [[nodiscard]] bool isPendingDestroy() const;

  sf::Vector2f position;
  sf::Vector2f offset;

  // Hybrid approach. Transitioning towards ECS
  SpriteComponent *spriteComponent = nullptr;

  GameObject(const GameObject &) = delete;
  GameObject &operator=(const GameObject &) = delete;

protected:
  bool pendingDestroy = false;

private:
  unsigned int id;
  static unsigned int nextId;
  UpdateDomain updateDomain;
};