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
        # is_selected: bool
        + draw(canvas: i_canvas)*
        + contains_point(x: double, y: double) bool*
        + get_bounding_box() bounding_box*
        + get_center() point
        + get_control_points() vector~control_point~
        + translate(delta: point)
        + has_fill() bool
        + get_type_tag() string*
    }

    class control_point {
        - position: point
        + get_position() point
        + set_position(p: point)
        + contains_point(x: double, y: double, tolerance: double) bool
    }

    class line {
        + draw(canvas: i_canvas)
        + contains_point(x: double, y: double) bool
        + get_bounding_box() bounding_box
        + has_fill() bool
    }

    class triangle {
        + draw(canvas: i_canvas)
        + contains_point(x: double, y: double) bool
        + get_bounding_box() bounding_box
    }

    class rectangle {
        + draw(canvas: i_canvas)
        + contains_point(x: double, y: double) bool
        + get_bounding_box() bounding_box
    }

    class ellipse {
        + draw(canvas: i_canvas)
        + contains_point(x: double, y: double) bool
        + get_bounding_box() bounding_box
    }

    class bezier_curve {
        + draw(canvas: i_canvas)
        + contains_point(x: double, y: double) bool
        + get_bounding_box() bounding_box
    }

    %% ----------------
    %% RELATIONSHIPS
    %% ----------------
    figure <|-- line
    figure <|-- triangle
    figure <|-- rectangle
    figure <|-- ellipse
    figure <|-- bezier_curve

    line          "1" *-- "2" control_point
    triangle      "1" *-- "3" control_point
    rectangle     "1" *-- "4" control_point : one per corner
    ellipse       "1" *-- "3" control_point : center + x-radius + y-radius
    bezier_curve  "1" *-- "*" control_point : n >= 2

    figure ..> i_canvas : draws to
```

## Notes

### `figure`

contains_point contract:
 - No fill: true if point is on border within a pixel tolerance
 - Has fill: true if point is inside filled area OR on border
has_fill() default returns true. line overrides to return false.
  Used by draw_ui() to show/hide the fill color picker.
  Used by contains_point() implementations to choose hit-test strategy.
get_center() is derived (centroid of control points), never stored.
get_type_tag() returns a fixed string per subclass
  (e.g. 'line', 'rectangle') used by scene_serializer
  to write the type into the .p_t file.

### `control_point`

contains_point used for drag hit-detection.
Each control_point is individually selectable
to deform the parent figure.

### `line`

has_fill() returns false — line is border only, never filled.
contains_point: true if distance from point
to segment is within a pixel tolerance.

### `rectangle`

Control points are the 4 corners.
Dragging one corner deforms the rectangle.
Ctrl held during creation forces a square.

### `ellipse`

3 control points: center, x-radius handle, y-radius handle.
Ctrl held during creation forces rx == ry (circle).

### `bezier_curve`

Stub: to be expanded once Bezier theory is covered.
degree = n_control_points - 1.

