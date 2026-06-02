#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

template <typename T> class MapLifecycle {
public:
  struct CreateEntry {
    std::string key;
    T object;
  };

  void queueCreate(std::string_view key, T object) {
    createQueue.push_back(
        CreateEntry{.key = std::string(key), .object = std::move(object)});
  }

  void queueDestroy(std::string key) { destroyQueue.push_back(std::move(key)); }

  void apply(std::unordered_map<std::string, T> &objects) {

    for (const auto &key : destroyQueue) {
      objects.erase(key);
    }

    for (auto &entry : createQueue) {
      objects.insert_or_assign(std::move(entry.key), std::move(entry.object));
    }

    createQueue.clear();
    destroyQueue.clear();
  }

private:
  std::vector<CreateEntry> createQueue;
  std::vector<std::string> destroyQueue;
};