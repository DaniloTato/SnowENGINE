#pragma once

#include <string>

struct GeneralContext;

template <typename OwnerType>
using ScriptFunc = void (*)(OwnerType &, const GeneralContext &);

template <typename OwnerType> struct NamedScript {
  std::string name;
  ScriptFunc<OwnerType> function;
  bool enabled = true;
};