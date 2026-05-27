#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

struct TransparentHash {
  using is_transparent = void;

  size_t operator()(std::string_view txt) const noexcept {
    return std::hash<std::string_view>{}(txt);
  }
};

struct TransparentEqual {
  using is_transparent = void;

  bool operator()(std::string_view lhs, std::string_view rhs) const noexcept {
    return lhs == rhs;
  }
};

class TextureManager {
public:
  static TextureManager &getInstance();

  sf::Texture &load(std::string_view id, std::string_view path);

  sf::Texture &get(std::string_view id);

private:
  std::unordered_map<std::string, sf::Texture, TransparentHash,
                     TransparentEqual>
      textures;

  TextureManager() = default;
};