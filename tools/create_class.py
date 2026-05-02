import sys
from pathlib import Path

if len(sys.argv) < 2:
    print("Usage: python create_class.py [ClassName]")
    exit()

class_name = sys.argv[1]

hpp = f"""\
#pragma once

class {class_name} {{
public:
    {class_name}() = default;
    ~{class_name}() = default;

private:

}};
"""

cpp = f"""\
#include "{class_name}.hpp"
"""

Path(f"include/{class_name}.hpp").write_text(hpp)
Path(f"src/{class_name}.cpp").write_text(cpp)

print(f"Created {class_name}.hpp and {class_name}.cpp")