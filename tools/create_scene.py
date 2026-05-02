import sys
from pathlib import Path

if len(sys.argv) < 2:
    print("Usage: python create_class.py [SceneName]")
    exit()

scene_name = sys.argv[1]

hpp = f"""\
#pragma once

namespace SceneBuilder {{
void {scene_name}Scene();
}}
"""

cpp = f"""\
#include "{scene_name}Scene.hpp"

#include "RegistryMacros.hpp"
#include "SceneBuilderRegistry.hpp" // IWYU pragma: keep

namespace SceneBuilder {{
void {scene_name}Scene() {{

  /* remember to build your context

    GeneralContext ctx = {{.playerPosition = &(player->position),
                          .textParams = dialogueManager.getAttachedTextParams(),
                          .player = player,
                          .bulletTexture = &bulletTexture}};
    GameState::getInstance().updateGeneralContext(ctx);

  */
}}
}}

REGISTER_SCENE_BUILDER("{scene_name}Scene",
                       SceneBuilder::{scene_name}Scene);
"""

Path(f"scene_builders/include/{scene_name}Scene.hpp").write_text(hpp)
Path(f"scene_builders/src/{scene_name}Scene.cpp").write_text(cpp)

print(f"Created {scene_name}Scene.hpp and {scene_name}Scene.cpp")