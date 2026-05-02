import sys
from pathlib import Path

TARGETS = {
    "GameCamera": {"include": "GameCamera.hpp", "var": "camera"},
    "GameObject": {"include": "GameObject.hpp", "var": "object"},
    "AnimatedObject": {"include": "AnimatedObject.hpp", "var": "animated"},
    "RenderableObject": {"include": "RenderableObject.hpp", "var": "renderable"},
    "TangibleObject": {"include": "TangibleObject.hpp", "var": "tangible"},
    "ScriptRunner": {"include": "ScriptRunner.hpp", "var": "runner"},
}

if len(sys.argv) < 3:
    print("Usage: python create_script.py [ScriptName] [TargetType]")
    print(f"Available types: {', '.join(TARGETS.keys())}")
    exit()

name = sys.argv[1]
target = sys.argv[2]

if target not in TARGETS:
    print(f"Invalid target type: {target}")
    exit()

func_name = name[0].lower() + name[1:]
state_name = name[0].upper() + name[1:] + "State"

target_include = TARGETS[target]["include"]
target_var = TARGETS[target]["var"]

#use of GeneralContext for unit testing

hpp = f"""\
#pragma once

#include "{target_include}"
#include "GeneralContext.hpp"

namespace Scripts {{

void {func_name}({target} &{target_var}, const GeneralContext &ctx);

}}
"""

cpp = f"""\
#include "{func_name}.hpp"

#include "{target_include}"

namespace Scripts {{

namespace {{

class {state_name} {{
public:
  void init() {{
    if (initialized) return;
    initialized = true;
  }}

  void update({target} &{target_var}, const GeneralContext &ctx) {{
    // TODO: implement behavior
  }}

private:
  bool initialized = false;
}};

}} // namespace

void {func_name}({target} &{target_var}, const GeneralContext &ctx) {{
  auto &state = {target_var}.scripter.getState<{state_name}>("{func_name}");

  state.init();
  state.update({target_var}, ctx);
}}

}} // namespace script
"""

include_path = Path(f"scripts/include/{func_name}.hpp")
src_path = Path(f"scripts/src/{func_name}.cpp")

include_path.parent.mkdir(parents=True, exist_ok=True)
src_path.parent.mkdir(parents=True, exist_ok=True)

include_path.write_text(hpp)
src_path.write_text(cpp)

print(f"Created {func_name}.hpp and {func_name}.cpp for {target}")