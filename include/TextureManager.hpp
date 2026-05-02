#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

class TextureManager {
public:
  static TextureManager &getInstance();

  sf::Texture &load(const std::string &id, const std::string &path);
  sf::Texture &get(const std::string &id);

private:
  std::unordered_map<std::string, sf::Texture> textures;

  TextureManager() = default;
};