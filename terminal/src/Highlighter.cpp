#include "Highlighter.hpp"

#include <string>
#include <vector>

std::string Highlighter::applyParenHighlight(const std::string &input) {
  static const std::vector<std::string> DEPTH_COLORS = {
      "#FFD700", "#00FFFF", "#7FFF00", "#FF69B4", "#FFA500", "#9370DB"};

  struct StackEntry {
    char ch;
    size_t index;
    int depth;
  };

  std::vector<int> depthAt(input.size(), -1);
  std::vector<bool> matched(input.size(), false);
  std::vector<StackEntry> stack;

  int depth = 0;

  auto isOpen = [](char c) { return c == '(' || c == '{'; };

  auto isClose = [](char c) { return c == ')' || c == '}'; };

  auto matches = [](char open, char close) {
    return (open == '(' && close == ')') || (open == '{' && close == '}');
  };

  for (size_t i = 0; i < input.size(); ++i) {
    char c = input[i];

    if (isOpen(c)) {
      stack.push_back({c, i, depth});
      depthAt[i] = depth;
      depth++;
    } else if (isClose(c)) {
      if (!stack.empty() && matches(stack.back().ch, c)) {
        depth--;
        auto open = stack.back();
        stack.pop_back();

        depthAt[i] = open.depth;
        matched[open.index] = matched[i] = true;
      }
    }
  }

  std::string out;

  for (size_t i = 0; i < input.size(); ++i) {
    char c = input[i];

    if (c == '(' || c == ')' || c == '{' || c == '}') {
      if (matched[i] && depthAt[i] >= 0) {
        const std::string &color =
            DEPTH_COLORS[depthAt[i] % DEPTH_COLORS.size()];

        out += "[color=" + color + "]";
        out += c;
        out += "[/color]";
      } else {
        out += "[color=#FF4444]";
        out += c;
        out += "[/color]";
      }
    } else {
      out += c;
    }
  }

  return out;
}