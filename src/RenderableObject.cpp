#include "RenderableObject.hpp"
#include "GameState.hpp"
#include "GeneralContext.hpp"

RenderableObject::RenderableObject(const RenderizerParameters &params)
    : GameObject({GameState::getInstance().windowPtrToType(&params.window)}),
      renderizer(params) {
  Renderizer::registerPair(this, &renderizer, params.registerAsRectShape);
}

void RenderableObject::update(const GeneralContext &ctx) {
  scripter.runScripts(*this, ctx);
}