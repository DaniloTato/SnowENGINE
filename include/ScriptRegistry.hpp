#pragma once

#include "Registry.hpp"

template <typename OwnerType>
class Scripter;

template <typename OwnerType>
class ScriptRegistry
    : public Registry<typename Scripter<OwnerType>::NamedScript> {};