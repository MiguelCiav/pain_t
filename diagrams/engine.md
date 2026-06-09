# Package: engine

Rendering infrastructure. Defines the `i_canvas` abstraction (now directly in `engine_2d` public API) and `engine_2d` class.
No dependencies on any other package in this project.

```mermaid
classDiagram
    class engine_2d {
        - window: GLFWwindow*
        - title: string
        - pixel_buffer: vector~color~
        - texture_id: GLuint
        - vao: GLuint
        - vbo: GLuint
        - ebo: GLuint
        - shader_program: GLuint
        - vs: GLuint
        - fs: GLuint
        - key_state: bool[GLFW_KEY_LAST]
        - mouse_button_state: bool[GLFW_MOUSE_BUTTON_LAST]
        - vertex_shader_source: char*
        - fragment_shader_source: char*
        - init()
        - setup_canvas()
        - upload_texture()
        - key_callback(window: GLFWwindow*, key: int, scancode: int, action: int, mods: int)$
        - mouse_button_callback(window: GLFWwindow*, button: int, action: int, mods: int)$
        - cursor_pos_callback(window: GLFWwindow*, xpos: double, ypos: double)$
        # width: int
        # height: int
        # get_mouse_position() vec2
        # is_key_pressed(key: int) bool
        # is_mouse_button_pressed(button: int) bool
        # clear(c: color)
        + engine_2d(width: int, height: int, title: string)
        + run()
        + on_key_down(key: int)*
        + on_key_up(key: int)*
        + on_mouse_button_down(button: int, x: double, y: double)*
        + on_mouse_button_up(button: int, x: double, y: double)*
        + on_mouse_move(x: double, y: double)*
        + setup()*
        + update(deltaTime: float)*
        + draw_ui()*
        + put_pixel(x: int, y: int, c: color)
        + get_width() int
        + get_height() int
    }
```

## Notes

### `engine_2d`

Subclass this to build an application.
The pixel buffer is a flat vector uploaded
to a full-screen OpenGL texture each frame.
Do NOT call put_pixel from draw_ui() — ImGui runs
after the texture is uploaded; pixel writes there are lost.
