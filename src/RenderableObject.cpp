#include "RenderableObject.hpp"
#include "GeneralContext.hpp"

RenderableObject::RenderableObject(const RenderizerParameters &params)
    : GameObject(UpdateDomain(params.window)), renderizer(params) {
  Renderizer::registerPair(this, &renderizer, params.registerAsRectShape);
}

void RenderableObject::update(const GeneralContext &ctx) {
  scripter.runScripts(*this, ctx);
}