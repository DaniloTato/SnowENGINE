#include "TextureManager.hpp"
#include <stdexcept>

TextureManager &TextureManager::getInstance() {
  static TextureManager instance;
  return instance;
}

sf::Texture &TextureManager::load(const std::string &id,
                                  const std::string &path) {
  auto &tex = textures[id];
  if (!tex.loadFromFile(path)) {
    throw std::runtime_error("Failed to load texture: " + path);
  }
  return tex;
}

sf::Texture &TextureManager::get(const std::string &id) {
  if (!textures.contains(id)) {
    throw std::runtime_error("Texture not found: " + id);
  }
  return textures.at(id);
}