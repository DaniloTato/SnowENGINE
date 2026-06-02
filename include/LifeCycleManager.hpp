#include <memory>
#include <vector>

template <typename T> class LifecycleManager {
public:
  using ObjectPtr = std::unique_ptr<T>;

  void queueCreate(ObjectPtr obj) { createQueue.push_back(std::move(obj)); }
  void queueDestroy(T *obj) { destroyQueue.push_back(obj); }

  void apply(std::vector<ObjectPtr> &objects) {
    std::erase_if(objects, [&](const auto &ptr) {
      return std::find(destroyQueue.begin(), destroyQueue.end(), ptr.get()) !=
             destroyQueue.end();
    });

    for (auto &obj : createQueue) {
      objects.push_back(std::move(obj));
    }

    createQueue.clear();
    destroyQueue.clear();
  }

private:
  std::vector<ObjectPtr> createQueue;
  std::vector<T *> destroyQueue;
};