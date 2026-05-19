#include "Registry.hpp"

#include "SceneManager.hpp"

class SceneBuilderRegistry : public Registry<SceneManager::SceneSetupFn> {};