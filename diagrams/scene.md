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
    class engine_2d
    class bounding_box { <<stub>> }

    class quad_tree {
        - bounds: bounding_box
        - elements: vector~figure*~
        - children: quad_tree*[4]
        - level: int
        - is_leaf() bool
        - create_children()
        - push_elements()
        + clear()
        + insert(fig: figure*)
        + remove(fig: figure*)
        + intersects(box: bounding_box) bool
        + query(bounds: bounding_box, result: unordered_set~figure*~*, visited_nodes: vector~bounding_box~*) unordered_set~figure*~
        + draw(engine: engine_2d*)
    }

    class scene {
        - history: command_history
        - background_color: color
        - active_border_color: color
        - active_fill_color: color
        - selected_figure: figure*
        - figures: vector~figure*~
        - active_bordered: bool
        - active_filled: bool
        - tree: quad_tree*
        - query_anim_nodes: vector~bounding_box~
        - query_anim_index: int
        - query_anim_timer: float
        + init_tree(x: double, y: double, width: double, height: double)
        + add_figure(f: figure*)
        + remove_figure(f: figure*)
        + notify_figure_moved(f: figure*)
        + clear()
        + reorder_figures(source_idx: int, target_idx: int)
        + get_figures() vector~figure*~
        + select(f: figure*)
        + deselect()
        + get_selected_figure() figure*
        + query(click: point) figure*
        + draw_all(engine: engine_2d*)
        + draw_quad_tree(engine: engine_2d*)
        + update_animation(deltaTime: float)
        + get_background_color() color
        + set_background_color(c: color)
        + get_active_border_color() color
        + set_active_border_color(c: color)
        + get_active_fill_color() color
        + set_active_fill_color(c: color)
        + is_active_bordered() bool
        + set_active_bordered(b: bool)
        + is_active_filled() bool
        + set_active_filled(f: bool)
        + undo()
        + redo()
        + execute(cmd: i_command*)
    }

    class ui_manager {
        + render(application: app*)
        - draw_tool_selector(application: app*)
        - draw_canvas_actions(application: app*)
        - draw_file_operations(application: app*)
        - draw_color_settings(application: app*)
        - draw_layers_panel(application: app*)
    }

    class shortcut_manager {
        - bindings: unordered_map~key_combo, function~void()~~
        + register_shortcut(key: int, ctrl: bool, action: function~void()~)
        + handle_key(key: int, ctrl: bool) bool
    }

    class app {
        - main_scene: scene
        - s_manager: shortcut_manager
        - active_tool: i_tool*
        - tools: vector~i_tool*~
        - show_quad_tree: bool
        - save_load_path: char[256]
        - status_message: string
        - status_color: color
        - status_timer: float
        - sidebar_width: double
        - gui: ui_manager
        - clipboard: figure*
        - set_status(msg: string, col: color)
        - register_shortcuts()
        - register_tools()
        + setup()
        + on_key_down(key: int)
        + on_mouse_button_down(button: int, x: double, y: double)
        + on_mouse_button_up(button: int, x: double, y: double)
        + on_mouse_move(x: double, y: double)
        + update(deltaTime: float)
        + draw_ui()
        + save_scene()
        + load_scene()
        + get_scene() scene&
        + get_active_tool() i_tool*
        + set_active_tool(tool: i_tool*)
        + get_tools() vector~i_tool*~
        + is_showing_quad_tree() bool
        + set_show_quad_tree(show: bool)
        + get_save_load_path() char*
        + get_status_message() string
        + get_status_color() color
        + get_sidebar_width() double
        + set_sidebar_width(width: double)
        + is_ctrl_pressed() bool
        + is_enter_pressed() bool
        + is_escape_pressed() bool
    }

    %% ----------------
    %% SERIALIZATION
    %% ----------------
    class scene_serializer {
        + save(s: scene&, filepath: string) bool$
        + load_into(filepath: string, s: scene&, engine: engine_2d*) bool$
    }

    engine_2d       <|-- app
    app             *-- scene
    app             *-- shortcut_manager
    app             *-- ui_manager
    app             o-- i_tool         : active_tool
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

init_tree(x, y, width, height): constructs quad_tree with
  bounding_box{x, y, width, height} as its root region.
draw_all renders figures sorted by z_index ascending.
draw_quad_tree recursively draws each quad_tree node bounding box.
  Designed to support an optional selection-search animation in the future.
execute() forwards to command_history and keeps quad_tree in sync.
clear(): removes all figures, clears quad_tree, deselects.
  Used by clear_scene_command::execute() and scene_serializer::load_into().
selected_figure is nullable (nullptr = nothing selected).
active_border_color / active_fill_color: the 'brush state'.
  Drawing tools read these in on_mouse_down() when constructing
  the preview figure, so new figures inherit the user's current color.
  Set by draw_ui() via scene.set_active_border/fill_color().

### `app`

Rendering order inside update() each frame:
  1. scene.draw_all(engine)          — background + committed figures
  2. if show_quad_tree:           — global toggle, Q key in on_key_down
        scene.draw_quad_tree(engine) — always up to date (real-time per spec)
  3. active_tool->draw_preview() — ghost or selection decorations on top
set_active_tool() calls reset() on the outgoing tool before switching.
All input events are blindly delegated to active_tool.
app never directly manages figures or commands.

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
