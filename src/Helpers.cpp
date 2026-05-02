#include "Helpers.hpp"
#include <cmath>
#include <unordered_map>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace Helper {
sf::Texture &loadTexture(const std::string &path) {
  static std::unordered_map<std::string, sf::Texture> cache;
  if (cache.find(path) == cache.end()) {
    sf::Texture tex;
    if (!tex.loadFromFile(path)) {
      throw std::runtime_error("Failed to load texture " + path);
    }
    cache[path] = tex;
  }
  return cache[path];
}

float distance(const sf::Vector2f &a, const sf::Vector2f &b) {
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  return std::sqrt(dx * dx + dy * dy);
}

sf::FloatRect makeRectFromPoints(float x1, float y1, float x2, float y2) {
  float left = std::min(x1, x2);
  float top = std::min(y1, y2);
  float width = std::abs(x2 - x1);
  float height = std::abs(y2 - y1);

  return {sf::FloatRect(left, top, width, height)};
}

std::filesystem::path getExecutableDir() {
#if defined(__APPLE__)
  uint32_t size = 0;
  _NSGetExecutablePath(nullptr, &size);
  std::string buffer(size, '\0');
  _NSGetExecutablePath(buffer.data(), &size);
  return std::filesystem::path(buffer).parent_path();

#elif defined(__linux__)
  char result[1024];
  ssize_t count = readlink("/proc/self/exe", result, sizeof(result));
  return std::filesystem::path(std::string(result, count)).parent_path();

#elif defined(_WIN32)
  char buffer[MAX_PATH];
  GetModuleFileNameA(nullptr, buffer, MAX_PATH);
  return std::filesystem::path(buffer).parent_path();
#endif
}

std::string getPath(const std::string &relativePath) {
  std::filesystem::path base = getExecutableDir();
  std::filesystem::path rel(relativePath);

  std::filesystem::path full = base / rel;

  return full.make_preferred().string();
}

float randRange(float min, float max) {
  return min + ((float)rand() / RAND_MAX) * (max - min);
}

} // namespace Helper