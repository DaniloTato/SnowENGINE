#pragma once
#include "RuntimeValue.hpp"
#include <memory>

namespace Snowlang {

class SnowlangInstance;
struct LambdaInstance;

struct Cell {

  struct Watcher {
    std::shared_ptr<LambdaInstance> lambda;
    SnowlangInstance *owner;
  };

  std::vector<Watcher> watchers;

  void setCellValue(RuntimeValue newValue);
  const RuntimeValue &getCellValue();
  RuntimeValue &getMutableCellValue();

  void notify();

private:
  RuntimeValue value;
};

using CellPtr = std::shared_ptr<Cell>;

} // namespace Snowlang