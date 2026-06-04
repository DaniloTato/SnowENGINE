#pragma once

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <vector>

template <typename Key, typename Object> class MapLifecycle {
public:
  struct CreateEntry {
    Key key;
    Object object;
  };

  void queueCreate(Key key, Object object) {
    createQueue.emplace_back(CreateEntry{std::move(key), std::move(object)});
  }

  void queueDestroy(Key key) { destroyQueue.push_back(std::move(key)); }

  void apply(std::unordered_map<Key, Object> &objects) {
    for (const auto &key : destroyQueue) {
      objects.erase(key);
    }
    for (auto &entry : createQueue) {
      assert(!objects.contains(entry.key));
      objects.emplace(std::move(entry.key), std::move(entry.object));
    }
    createQueue.clear();
    destroyQueue.clear();
  }

private:
  std::vector<CreateEntry> createQueue;
  std::vector<Key> destroyQueue;
};