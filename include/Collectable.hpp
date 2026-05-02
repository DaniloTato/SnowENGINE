#pragma once
#include "TangibleObject.hpp"

class Collectable : public TangibleObject {
public:
  Collectable(RenderizerParameters params)
      : TangibleObject(params, Animations{}) {}
  // may change the constructor to ask for animations set later

  bool collected = false;
};