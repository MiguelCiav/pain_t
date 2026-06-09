# pain_t — Architecture Overview

High-level package dependency map. See `diagrams/` for detailed class diagrams per package.

```mermaid
graph LR
    engine["📦 engine
    ─────────────
    i_canvas
    engine_2d"]

    figures["📦 figures
    ─────────────
    figure · point
    bounding_box · control_point
    line · triangle
    rectangle · ellipse
    bezier"]

    scene_pkg["📦 scene
    ─────────────
    scene · app
    quad_tree
    scene_serializer"]

    tools["📦 tools
    ─────────────
    i_tool
    selection_tool
    line_tool · rect_tool
    ellipse_tool
    triangle_tool · bezier_tool"]

    commands["📦 commands
    ─────────────
    i_command
    command_history
    + 12 command classes"]

    figures     -->|"figure::draw uses"| engine
    scene_pkg   -->|"owns collection of"| figures
    scene_pkg   -->|"owns"| commands
    tools       -->|"creates & previews"| figures
    tools       -->|"queries & executes on"| scene_pkg
    commands    -->|"captures references to"| figures
    commands    -->|"calls add/remove on"| scene_pkg
```

## Package → File Map

| Package | Detail diagram |
|---|---|
| `engine` | [diagrams/engine.md](diagrams/engine.md) |
| `figures` | [diagrams/figures.md](diagrams/figures.md) |
| `scene` | [diagrams/scene.md](diagrams/scene.md) |
| `tools` | [diagrams/tools.md](diagrams/tools.md) |
| `commands` | [diagrams/commands.md](diagrams/commands.md) |

## Dependency Rules

- `engine` has **no** dependencies on other packages.
- `figures` depends only on `engine` (via `i_canvas`).
- `commands` and `tools` depend on `figures` and `scene`, never on each other.
- `scene` depends on `figures` and `commands` but **not** on `tools` — tools call into scene, not the other way around.

## Building and Running

### Prerequisites
- CMake 3.15 or higher
- C++20 compliant compiler (GCC 10+, Clang 10+, MSVC 2019+)
- OpenGL development libraries (e.g. `libx11-dev`, `libxrandr-dev`, `libxinerama-dev`, `libxcursor-dev`, `libxi-dev` on Linux)

### Building the Project
```bash
# Generate build configuration
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Compile the application
cmake --build build -j$(nproc)
```

### Running the Application
```bash
./build/pain_t/pain_t
```