#include "RenderableObject.hpp"
#include "GeneralContext.hpp"

RenderableObject::RenderableObject(const RenderizerParameters &params)
    : GameObject(UpdateDomain(params.window)) {}

void RenderableObject::update(const GeneralContext &ctx) {
  scripter.runScripts(*this, ctx);
}