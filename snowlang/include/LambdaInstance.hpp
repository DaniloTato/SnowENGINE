#pragma once
#include "ResolvedAST.hpp"
#include <memory>

namespace Snowlang {

struct Cell;
using CellPtr = std::shared_ptr<Cell>;

struct LambdaInstance {
  int id;
  std::vector<size_t> paramSlots;
  std::vector<RStmtPtr> body;
  int frameSize;

  std::vector<CellPtr> captures;

  LambdaInstance(int id, std::vector<size_t> params, std::vector<RStmtPtr> body, int frameSize,
                 std::vector<CellPtr> captures)
      : id(id), paramSlots(std::move(params)), body(std::move(body)), frameSize(frameSize),
        captures(std::move(captures)) {}
};

} // namespace Snowlang