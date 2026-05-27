#include "TextureManager.hpp"

#include <stdexcept>

TextureManager &TextureManager::getInstance() {
  static TextureManager instance;
  return instance;
}

sf::Texture &TextureManager::load(std::string_view id, std::string_view path) {
  auto [it, inserted] = textures.try_emplace(std::string(id));
  auto &tex = it->second;

  if (!tex.loadFromFile(std::string(path))) {
    throw std::runtime_error("Failed to load texture: " + std::string(path));
  }

  return tex;
}

sf::Texture &TextureManager::get(std::string_view id) {

  auto it = textures.find(id);

  if (it == textures.end()) {
    throw std::runtime_error("Texture not found: " + std::string(id));
  }

  return it->second;
}