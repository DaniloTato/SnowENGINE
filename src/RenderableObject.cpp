#include "RenderableObject.hpp"
#include "GeneralContext.hpp"

RenderableObject::RenderableObject(const RenderizerParameters &params)
    : GameObject(UpdateDomain(WindowManager::Set::MAIN)) {}

void RenderableObject::update(const GeneralContext &ctx) {
  scripter.runScripts(*this, ctx);
}