# Package: tools

Interaction FSM (GoF State pattern). Each tool is a state object.
Depends on: `figures`, `scene`, `engine` (via `i_canvas`).

```mermaid
classDiagram
    %% External stubs
    class i_canvas { <<interface>> }
    class scene
    class figure { <<abstract>> }
    class line
    class rectangle
    class ellipse
    class triangle
    class bezier_curve

    class i_tool {
        <<interface>>
        # engine_2d* engine
        # app* application
        + on_mouse_down(button: int, p: point)*
        + on_mouse_move(p: point)*
        + on_mouse_up(button: int, p: point)*
        + on_key_down(key: int)*
        + reset()
        + draw_preview()*
        + draw_settings()
        + get_name() string*
        + get_label() string*
        + get_icon() string*
    }

    class selection_tool {
        - is_dragging: bool
        - last_mouse_point: point
        - cumulative_shift: point
        - is_deforming: bool
        - active_control_point_idx: int
        - deform_start_pos: point
        - deform_start_points: vector~point~
        - subdivision_t: float
        + on_mouse_down(button: int, p: point)
        + on_mouse_move(p: point)
        + on_mouse_up(button: int, p: point)
        + on_key_down(key: int)
        + reset()
        + draw_preview()
        + draw_settings()
        + get_name() string
        + get_label() string
        + get_icon() string
    }

    class line_tool {
        - is_drawing: bool
        - starting_point: point
        - ending_point: point
        + on_mouse_down(button: int, p: point)
        + on_mouse_move(p: point)
        + on_mouse_up(button: int, p: point)
        + on_key_down(key: int)
        + reset()
        + draw_preview()
        + get_name() string
        + get_label() string
        + get_icon() string
    }

    class rect_tool {
        - is_drawing: bool
        - starting_point: point
        - ending_point: point
        + on_mouse_down(button: int, p: point)
        + on_mouse_move(p: point)
        + on_mouse_up(button: int, p: point)
        + on_key_down(key: int)
        + reset()
        + draw_preview()
        + get_name() string
        + get_label() string
        + get_icon() string
    }

    class ellipse_tool {
        - is_drawing: bool
        - starting_point: point
        - ending_point: point
        + on_mouse_down(button: int, p: point)
        + on_mouse_move(p: point)
        + on_mouse_up(button: int, p: point)
        + on_key_down(key: int)
        + reset()
        + draw_preview()
        + get_name() string
        + get_label() string
        + get_icon() string
    }

    class triangle_tool {
        - state: int
        - p1: point
        - p2: point
        - p3: point
        + on_mouse_down(button: int, p: point)
        + on_mouse_move(p: point)
        + on_mouse_up(button: int, p: point)
        + on_key_down(key: int)
        + reset()
        + draw_preview()
        + get_name() string
        + get_label() string
        + get_icon() string
    }

    class bezier_tool {
        - is_drawing: bool
        - points: vector~point~
        - current_mouse_pos: point
        + on_mouse_down(button: int, p: point)
        + on_mouse_move(p: point)
        + on_mouse_up(button: int, p: point)
        + on_key_down(key: int)
        + reset()
        + draw_preview()
        + get_name() string
        + get_label() string
        + get_icon() string
    }

    i_tool <|.. selection_tool
    i_tool <|.. line_tool
    i_tool <|.. rect_tool
    i_tool <|.. ellipse_tool
    i_tool <|.. triangle_tool
    i_tool <|.. bezier_tool

    line_tool     *-- line        : preview
    rect_tool     *-- rectangle   : preview
    ellipse_tool  *-- ellipse     : preview
    triangle_tool *-- triangle    : preview
    bezier_tool   *-- bezier_curve : preview
```

## Notes

### `i_tool`

pain_t delegates all input events here.
Each tool reads and mutates scene only through
scene's public API (query, select, execute).
reset() is called by pain_t::set_active_tool() before
the old tool is replaced — clears preview and resets
internal phase back to its default state.

### `selection_tool`

Internal selection_phase FSM:
 IDLE
  └─ click figure ──────────────► FIGURE_SELECTED
 FIGURE_SELECTED
  ├─ drag center handle ────────► DRAGGING_FIGURE
  └─ drag control point ────────► DRAGGING_CONTROL_POINT
 DRAGGING_FIGURE
  └─ mouse up → push move_figure_command → FIGURE_SELECTED
 DRAGGING_CONTROL_POINT
  └─ mouse up → push move_control_point_command → FIGURE_SELECTED
dragged_cp tracks the specific control_point being moved.
draw_preview() renders selection decorations only:
 - border highlight on selected figure
 - small square handle at each control_point position
 - extra circle handle at get_center() for whole-figure move
Quad tree overlay is NOT rendered here — it is global
state managed by pain_t, drawn between draw_all() and draw_preview().

### `line_tool`

on_mouse_down: instantiate preview.
on_mouse_move: update endpoint of preview.
on_mouse_up: push create_figure_command, clear preview.
draw_preview() renders preview with a fixed PREVIEW_COLOR
(distinct solid color — no alpha blending needed).
The ghost shows border only, no fill.

### `rect_tool`

constrain toggled by on_key_down/up(CTRL).
When true, forces width == height (square).

### `ellipse_tool`

constrain toggled by on_key_down/up(CTRL).
When true, forces rx == ry (circle).
