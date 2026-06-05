#pragma once
#include "Debug.hpp"
#include "Engine.hpp"
#include "Evaluator.hpp"
#include "LevelManager.hpp"
#include "Memory.hpp"
#include "Resolver.hpp"
#include "SnowIO.hpp"

#include "GameObjectExposure.hpp"

#include "ScheduledTask.hpp"
#include <string>

namespace Snowlang {

class SnowlangInstance {
private:
  struct Module {
    std::unordered_map<std::string, size_t> exportSlots;
  };

private:
  std::string cachedSource;
  void cacheSource(const std::string &source);

  std::vector<ScheduledTask> scheduledTasks;
  size_t nextTaskId = 0;
  void bindGameState();
  static std::unique_ptr<SnowlangInstance> latestInstance;

public:
  SnowlangInstance(SnowIO &ioInterface);
  void run(const std::string &source);
  [[nodiscard]] std::string readFile(const std::string &path) const;
  Module loadModule(const std::string &path);
  void update(double deltaTime);
  [[nodiscard]] const std::vector<ScheduledTask> &getTasks() const;

  void scheduleTask(ScheduledTask &&task);
  bool togglePauseTask(size_t id);
  bool killTask(size_t id);
  void killAllTasks();
  [[nodiscard]] size_t taskCount() const;

  size_t getNewTaskId();

  ObjectRef adaptDescriptor(const GameObjectExposure::Descriptor &desc);
  RuntimeValue adaptValue(const GameObjectExposure::Value &v);
  GameObjectExposure::Value adaptBack(const RuntimeValue &rv);

  static SnowlangInstance &getLatestSnowlangInstance();

public:
  Memory memory;
  Resolver resolver;
  Evaluator evaluator;
  SnowIO &io;
  Debug debug;

  // Quick fix for now. Correct Later
  Engine *engineRef;
};

} // namespace Snowlang