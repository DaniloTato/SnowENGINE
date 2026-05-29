#pragma once

#include <memory>
#include <vector>

class GameObject;
struct GeneralContext;

class Scene {
public:
  virtual ~Scene() = default;

  virtual void setup() {}
  virtual void update(const GeneralContext &ctx);

  template <typename T, typename... Args> T *createObject(Args &&...args);

  void destroyObject(GameObject *obj);

  [[nodiscard]] const std::vector<std::unique_ptr<GameObject>> &
  getObjects() const;

protected:
  std::vector<std::unique_ptr<GameObject>> objects;
};