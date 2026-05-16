# Package: engine

Rendering infrastructure. Defines the `i_canvas` abstraction and its sole concrete implementation `engine_2d`.
No dependencies on any other package in this project.

```mermaid
classDiagram
    class i_canvas {
        <<interface>>
        + put_pixel(x: int, y: int, c: color)*
        + clear(c: color)*
        + get_width() int*
        + get_height() int*
    }

    class engine_2d {
        # width: int
        # height: int
        + put_pixel(x: int, y: int, c: color)
        + clear(c: color)
        + get_width() int
        + get_height() int
        + run()
        + setup()*
        + update(delta_time: float)*
        + draw_ui()*
        + on_key_down(key: int)*
        + on_key_up(key: int)*
        + on_mouse_button_down(button: int, x: double, y: double)*
        + on_mouse_button_up(button: int, x: double, y: double)*
        + on_mouse_move(x: double, y: double)*
        # is_key_pressed(key: int) bool
        # is_mouse_button_pressed(button: int) bool
        # get_mouse_position() point
    }

    i_canvas <|.. engine_2d
```

## Notes

### `engine_2d`

Subclass this to build an application.
The pixel buffer is a flat vector uploaded
to a full-screen OpenGL texture each frame.
Do NOT call put_pixel from draw_ui() — ImGui runs
after the texture is uploaded; pixel writes there are lost.
put_pixel and clear are shown as public because i_canvas
requires them at that visibility. In the existing code they
are protected and will be promoted when i_canvas is introduced.

