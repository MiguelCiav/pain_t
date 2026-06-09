# Package: figures

Domain model. All 2D primitives and their control points.
Depends on: `engine` (via `i_canvas`).

```mermaid
classDiagram
    %% -----------------------------------------------
    %% External stub — defined in: diagrams/engine.md
    %% -----------------------------------------------
    class i_canvas {
        <<interface>>
    }

    %% ----------------
    %% FIGURES
    %% ----------------
    class figure {
        <<abstract>>
        # z_index: int
        # border_color: color
        # fill_color: color
        # selected: bool
        # filled: bool
        # bordered: bool
        # engine: engine_2d*
        # control_points: vector~control_point~
        + get_z_index() int
        + get_border_color() color
        + get_fill_color() color
        + is_selected() bool
        + is_filled() bool
        + is_bordered() bool
        + can_fill() bool
        + get_center() point
        + get_bounding_box() bounding_box
        + get_control_points() vector~control_point~
        + set_control_point(index: size_t, p: point)
        + set_engine(engine: engine_2d*)
        + set_z_index(z_index: int)
        + set_border_color(border_color: color)
        + set_fill_color(fill_color: color)
        + set_bordered(bordered: bool)
        + set_filled(filled: bool)
        + select()
        + unselect()
        + move(shift: point)
        + draw()
        + draw_border()*
        + draw_fill()*
        + inside(click: point) bool
        + get_type_tag() string*
        + clone() figure*
        + scale(factor: double)
    }

    class point {
        + x: double
        + y: double
    }

    class bounding_box {
        - points: vector~point~
        + get_bounding_box() vector~point~
        + get_center() point
        + set_points(p1: point, p2: point, p3: point, p4: point)
        + set_points(points: vector~point~)
    }

    class control_point {
        - position: point
        + get_position() point
        + get_x() double
        + get_y() double
        + set_position(position: point)
        + set_position(x: double, y: double)
        + set_x(x: double)
        + set_y(y: double)
        + contains_point(p: point, tolerance: double) bool
        + contains_point(x: double, y: double, tolerance: double) bool
    }

    class line {
        + can_fill() bool
        + draw_fill()
        + draw_border()
        + on_border(click: point) bool
        + on_filling(click: point) bool
        + get_type_tag() string
        + clone() figure*
    }

    class triangle {
        + can_fill() bool
        + draw_fill()
        + draw_border()
        + on_border(click: point) bool
        + on_filling(click: point) bool
        + get_type_tag() string
        + clone() figure*
    }

    class rectangle {
        + can_fill() bool
        + draw_fill()
        + draw_border()
        + on_border(click: point) bool
        + on_filling(click: point) bool
        + get_type_tag() string
        + clone() figure*
    }

    class ellipse {
        + can_fill() bool
        + draw_fill()
        + draw_border()
        + on_border(click: point) bool
        + on_filling(click: point) bool
        + get_bounding_box() bounding_box
        + get_center() point
        + get_type_tag() string
        + clone() figure*
        + set_control_point(index: size_t, p: point)
    }

    class bezier {
        - get_render_points() vector~point~
        + can_fill() bool
        + draw_fill()
        + draw_border()
        + on_border(click: point) bool
        + on_filling(click: point) bool
        + get_type_tag() string
        + clone() figure*
        + increase_degree()
        + evaluate(t: double) point
        + subdivide(t: double) pair~vector~point~, vector~point~~
    }

    %% ----------------
    %% RELATIONSHIPS
    %% ----------------
    figure <|-- line
    figure <|-- triangle
    figure <|-- rectangle
    figure <|-- ellipse
    figure <|-- bezier

    line          "1" *-- "2" control_point
    triangle      "1" *-- "3" control_point
    rectangle     "1" *-- "4" control_point : one per corner
    ellipse       "1" *-- "3" control_point : center + x-radius + y-radius
    bezier        "1" *-- "*" control_point : n >= 2

    figure ..> i_canvas : draws to
```

## Notes

### `figure`

contains_point contract:
 - No fill: true if point is on border within a pixel tolerance
 - Has fill: true if point is inside filled area OR on border
can_fill() default returns true. line overrides to return false.
  Used by draw_ui() to show/hide the fill color picker.
  Used by inside() implementations to choose hit-test strategy.
get_center() is derived (centroid of control points), never stored.
get_type_tag() returns a fixed string per subclass
  (e.g. 'line', 'rectangle') used by scene_serializer
  to write the type into the .p_t file.

### `point`

A simple geometric coordinate structure to avoid
polluting `figures` with external math libraries.

### `bounding_box`

Used internally by quad_tree for subdivision
and range query logic. Every figure computes
its own bounds.

### `control_point`

contains_point used for drag hit-detection.
Each control_point is individually selectable
to deform the parent figure.

### `line`

can_fill() returns false — line is border only, never filled.
on_border: true if distance from point
to segment is within a pixel tolerance.

### `rectangle`

Control points are the 4 corners.
Dragging one corner deforms the rectangle.
Ctrl held during creation forces a square.

### `ellipse`

3 control points: center, x-radius handle, y-radius handle.
Ctrl held during creation forces rx == ry (circle).

### `bezier`

degree = n_control_points - 1.
Can be subdivided into two independent bezier curves.
