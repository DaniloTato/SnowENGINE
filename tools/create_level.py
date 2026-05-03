import argparse
from pathlib import Path

parser = argparse.ArgumentParser(description="Create a new level file")

parser.add_argument("level_name", help="Name of the level")
parser.add_argument("tileset_path", help="Path to the tileset")

parser.add_argument(
    "--tile-size",
    type=int,
    default=16,
    help="Tile size (default: 16)"
)

args = parser.parse_args()

json_content = f"""\
{{
    "tile_size": {args.tile_size},
    "tileset": "{args.tileset_path}",
    "layers": [
        {{
            "name": "default",
            "parallax": 1.0,
            "tiles": [
                {{
                    "tex_x": 0,
                    "tex_y": 0,
                    "x": 100,
                    "y": 100
                }}
            ]
        }}
    ]
}}
"""

Path(f"assets/levels/{args.level_name}.json").write_text(json_content)

print(f"Created {args.level_name}.json with tile size {args.tile_size}")