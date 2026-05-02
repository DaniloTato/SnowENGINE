#include "Cell.hpp"
#include "SnowlangInstance.hpp"

namespace Snowlang {

void Cell::setCellValue(RuntimeValue newValue) {
  value = std::move(newValue);
  notify();
}

const RuntimeValue &Cell::getCellValue() { return value; }

RuntimeValue &Cell::getMutableCellValue() { return value; }

void Cell::notify() {
  for (auto &watcher : watchers) {
    watcher.owner->evaluator.runLambda(watcher.lambda);
  }
}

} // namespace Snowlang