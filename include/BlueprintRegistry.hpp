#pragma once

#include "Registry.hpp"
#include "TangibleObject.hpp"

class BlueprintRegistry
    : public Registry<std::function<TangibleObject *(const sf::Vector2f &)>> {};