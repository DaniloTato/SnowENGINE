#pragma once

#include "Tokenizer.hpp"

namespace Snowlang {

struct Node {
  SourceSpan span;
  virtual ~Node() = default;
};

} // namespace Snowlang