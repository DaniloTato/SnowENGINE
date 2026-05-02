#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

template <typename T> class Cache {
protected:
  std::unordered_map<std::string, T> cache;

public:
  void load(const std::string &id, const T &obj) { cache[id] = obj; }

  T &get(const std::string &id) { return cache.at(id); }
};