#include <cstddef>

struct CameraID {
  size_t value = 0;

  bool operator==(const CameraID &other) const { return value == other.value; }

  bool operator!=(const CameraID &other) const { return value != other.value; }
};