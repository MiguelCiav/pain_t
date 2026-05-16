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
    figure
    control_point
    line · triangle
    rectangle · ellipse
    bezier_curve"]

    scene_pkg["📦 scene
    ─────────────
    scene · pain_t
    bounding_box · quad_tree
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
    + 8 command classes"]

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