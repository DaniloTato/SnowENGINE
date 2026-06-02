#pragma once
#include <string>

class Highlighter {
public:
  [[nodiscard]] std::string applyParenHighlight(const std::string &input) const;
};