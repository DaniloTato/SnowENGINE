#pragma once
#include "LambdaInstance.hpp"
#include <sstream>

namespace Snowlang {

struct ScheduledTask {
  size_t id;
  std::shared_ptr<LambdaInstance> lambda;
  double interval;
  double accumulator;

  ScheduledTask(size_t id, std::shared_ptr<LambdaInstance> lambda, double interval)
      : id(id), lambda(std::move(lambda)), interval(interval) {}

  void togglePause() { paused = !paused; }

  [[nodiscard]] bool isPaused() const { return paused; }

  [[nodiscard]] std::string describe() const {
    std::ostringstream ss;
    ss << "[" << id << "] " << (paused ? "paused" : "running") << " every " << interval << "s "
       << "<lambda @" << &lambda << ">";
    return ss.str();
  }

private:
  bool paused = false;
};

} // namespace Snowlang