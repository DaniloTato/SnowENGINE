#pragma once
#include "GameObjectExposure.hpp"
#include "SFML/System/Vector2.hpp"
#include "WindowManager.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

struct GeneralContext;
class SpriteComponent;
class CameraComponent;
class RenderProvider;
class TextRenderComponent;
class TextComponent;

class GameObject {
public:
  struct UpdateDomain {
    std::vector<WindowID> windows;
    std::vector<WindowManager::Set> domains;

    static const UpdateDomain Universal;

    bool matches(WindowManager &wm, WindowID id) const;

    UpdateDomain(WindowID id) : windows({id}) {}

    UpdateDomain(std::vector<WindowID> &&ids) : windows(std::move(ids)) {}

    UpdateDomain(WindowManager::Set domain) : domains({domain}) {}

    UpdateDomain(std::vector<WindowManager::Set> &&doms)
        : domains(std::move(doms)) {}
  };

  struct ID {
    ID(u_int32_t val) : val(val) {}
    u_int32_t val;
    auto operator<=>(const ID &) const = default;
  };

  GameObject(UpdateDomain updateDomain, sf::Vector2f pos = {0.f, 0.f});
  virtual ~GameObject() = default;
  virtual void update(const GeneralContext &ctx) {}

  virtual std::shared_ptr<GameObjectExposure::Descriptor> describe();
  GameObject::ID getId();
  bool isUpdateDomainPaused(WindowManager &wm);

  void setUpdateDomain(const UpdateDomain &newDomain);

  void destroyLater();
  [[nodiscard]] bool isPendingDestroy() const;

  sf::Vector2f position;
  sf::Vector2f offset;

  // Hybrid approach. Transitioning towards ECS

  // Right now it can only hold one sprite component.
  // Next, let's move towards vector<RenderPrivoder>
  std::vector<RenderProvider *> renderProviders;
  CameraComponent *cameraComponent = nullptr;
  SpriteComponent *spriteComponent = nullptr;
  TextRenderComponent *textRenderComponent = nullptr;
  TextComponent *textComponent = nullptr;

  GameObject(const GameObject &) = delete;
  GameObject &operator=(const GameObject &) = delete;

protected:
  bool pendingDestroy = false;

private:
  ID id;
  static unsigned int nextId;
  UpdateDomain updateDomain;
};

namespace std {

template <>

struct hash<GameObject::ID> {

  size_t operator()(const GameObject::ID &id) const noexcept {

    return hash<uint32_t>{}(id.val);
  }
};

} // namespace std