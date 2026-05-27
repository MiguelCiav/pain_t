# Package: scene

World state, spatial index, and application entry point.
Depends on: `figures`, `commands`, `engine`, `tools` (via `i_tool` pointer only).

```mermaid
classDiagram
    %% External stubs
    class figure { <<abstract>> }
    class i_command { <<interface>> }
    class command_history
    class i_tool { <<interface>> }
    class i_canvas { <<interface>> }
    class engine_2d
    class bounding_box { <<stub>> }

    class quad_tree {
        - bounds: bounding_box
        + quad_tree(bounds: bounding_box)
        + insert(f: figure*)
        + remove(f: figure*)
        + query(range: bounding_box) vector~figure*~
        + clear()
    }

    class scene {
        - figures: vector~shared_ptr~figure~~
        - quad_tree: quad_tree
        - background_color: color
        - history: command_history
        - selected_figure: figure*
        - active_border_color: color
        - active_fill_color: color
        + scene(width: int, height: int)
        + draw_all(canvas: i_canvas)
        + draw_quad_tree(canvas: i_canvas)
        + add_figure(f: shared_ptr~figure~)
        + remove_figure(f: shared_ptr~figure~)
        + notify_figure_moved(f: figure*)
        + get_all_figures() vector~shared_ptr~figure~~
        + clear_all_figures()
        + query(x: double, y: double) figure*
        + select(f: figure*)
        + deselect()
        + get_selected_figure() figure*
        + set_background_color(c: color)
        + get_background_color() color
        + set_active_border_color(c: color)
        + get_active_border_color() color
        + set_active_fill_color(c: color)
        + get_active_fill_color() color
        + execute(cmd: shared_ptr~i_command~)
        + undo()
        + redo()
    }

    class pain_t {
        - scene: scene
        - active_tool: i_tool*
        - quad_tree_visible: bool
        + setup()
        + update(delta_time: float)
        + draw_ui()
        + set_active_tool(tool: i_tool*)
        + on_key_down(key: int)
        + on_key_up(key: int)
        + on_mouse_button_down(button: int, x: double, y: double)
        + on_mouse_button_up(button: int, x: double, y: double)
        + on_mouse_move(x: double, y: double)
    }

    %% ----------------
    %% SERIALIZATION
    %% ----------------
    class scene_serializer {
        + save(s: scene&, filepath: string) bool
        + load_into(filepath: string, s: scene&) bool
    }

    engine_2d       <|-- pain_t
    pain_t          *-- scene
    pain_t          o-- i_tool         : active_tool
    scene           *-- command_history
    scene           o-- figure         : selected_figure
    scene           *-- quad_tree
    quad_tree       "1" *-- "4" quad_tree : subdivisions
    command_history o-- i_command
    scene_serializer ..> scene         : reads and replaces
```

## Notes

### `quad_tree`

Constructed with the full canvas bounding_box as its root region.
Subdivides into 4 children when a node exceeds capacity.
All queries and insertions are bounded by this root region.

### `scene`

scene(width, height): constructs quad_tree with
  bounding_box{0, 0, width, height} as its root region.
draw_all renders figures sorted by z_index ascending.
draw_quad_tree recursively draws each quad_tree node bounding box.
  Designed to support an optional selection-search animation in the future.
execute() forwards to command_history and keeps quad_tree in sync.
clear_all_figures(): removes all figures, clears quad_tree, deselects.
  Used by clear_canvas_command::execute() and scene_serializer::load_into().
selected_figure is nullable (nullptr = nothing selected).
active_border_color / active_fill_color: the 'brush state'.
  Drawing tools read these in on_mouse_down() when constructing
  the preview figure, so new figures inherit the user's current color.
  Set by draw_ui() via scene.set_active_border/fill_color().

### `pain_t`

Rendering order inside update() each frame:
  1. scene.draw_all(canvas)          — background + committed figures
  2. if quad_tree_visible:           — global toggle, Q key in on_key_down
        scene.draw_quad_tree(canvas) — always up to date (real-time per spec)
  3. active_tool->draw_preview(canvas) — ghost or selection decorations on top
set_active_tool() calls reset() on the outgoing tool before switching.
All input events are blindly delegated to active_tool.
pain_t never directly manages figures or commands.

### `scene_serializer`

.p_t file format (plain text, UTF-8):
  Line 1:  'pain_t v1'            — magic header + version
  Line 2:  'background R G B'     — canvas background color
  Blank line between figures.
  Per figure block:
    'figure <type_tag>'           — from figure::get_type_tag()
    'z_index <N>'
    'border_color R G B'
    'fill_color R G B'
    'control_points <N>'
    'cp X Y'  (one line per point)

save() returns false on I/O error.
load_into() clears scene entirely (figures + quad_tree +
  command_history) before loading new state.
Returns false if the file is missing or the header is wrong version.

