#include "Registry.hpp"

class SceneBuilderRegistry : public Registry<std::function<void()>> {};