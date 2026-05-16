# Package: commands

Undo/redo system (GoF Command pattern).
Depends on: `figures`, `scene`.

```mermaid
classDiagram
    %% External stubs
    class figure { <<abstract>> }
    class control_point
    class scene

    class i_command {
        <<interface>>
        + execute()
        + undo()
    }

    class command_history {
        - undo_stack: stack~shared_ptr~i_command~~
        - redo_stack: stack~shared_ptr~i_command~~
        + add(cmd: shared_ptr~i_command~)
        + undo()
        + redo()
        + can_undo() bool
        + can_redo() bool
    }

    class create_figure_command {
        - scene: scene&
        - figure: shared_ptr~figure~
    }

    class delete_figure_command {
        - scene: scene&
        - figure: shared_ptr~figure~
    }

    class move_figure_command {
        - scene: scene&
        - figure: shared_ptr~figure~
        - delta: point
    }

    class move_control_point_command {
        - scene: scene&
        - figure: shared_ptr~figure~
        - cp: control_point*
        - old_pos: point
        - new_pos: point
    }

    class set_color_command {
        - figure: shared_ptr~figure~
        - old_border: color
        - new_border: color
        - old_fill: color
        - new_fill: color
    }

    class set_background_color_command {
        - scene: scene&
        - old_color: color
        - new_color: color
    }

    class change_z_index_command {
        - scene: scene&
        - figure: shared_ptr~figure~
        - old_z: int
        - new_z: int
    }

    class clear_canvas_command {
        - scene: scene&
        - snapshot: vector~shared_ptr~figure~~
    }

    i_command <|-- create_figure_command
    i_command <|-- delete_figure_command
    i_command <|-- move_figure_command
    i_command <|-- move_control_point_command
    i_command <|-- set_color_command
    i_command <|-- set_background_color_command
    i_command <|-- change_z_index_command
    i_command <|-- clear_canvas_command

    command_history o-- i_command

    create_figure_command          --> scene
    delete_figure_command          --> scene
    move_figure_command            --> scene
    set_background_color_command   --> scene
    change_z_index_command         --> scene
    clear_canvas_command           --> scene
    move_control_point_command     --> control_point
    set_color_command              --> figure
```

## Notes

### `i_command`

No separate redo() method.
execute() IS redo — same operation replayed.
All implementations must be fully reversible.

### `command_history`

add() clears redo_stack — branching history is not supported.
undo(): pop undo_stack → call undo() → push to redo_stack.
redo(): pop redo_stack → call execute() → push to undo_stack.
can_undo/redo() used by draw_ui() to enable/disable buttons.

### `create_figure_command`

execute(): scene.add_figure(figure)
undo():    scene.remove_figure(figure)

### `delete_figure_command`

execute(): scene.remove_figure(figure)
undo():    scene.add_figure(figure)
Keeps shared_ptr alive so figure survives deletion.

### `move_figure_command`

Used when dragging the center handle (whole figure move).
execute(): figure.translate(+delta)
           scene.notify_figure_moved(figure)
undo():    figure.translate(-delta)
           scene.notify_figure_moved(figure)
One command pushed per complete drag (on mouse_up), not per pixel.
shared_ptr keeps figure alive through delete/undo sequences.

### `move_control_point_command`

Used when dragging an individual control point (deformation).
execute(): cp.set_position(new_pos)
           scene.notify_figure_moved(figure)
undo():    cp.set_position(old_pos)
           scene.notify_figure_moved(figure)
One command pushed per complete drag (on mouse_up), not per pixel.
cp* is safe: owned by figure, which is kept alive by shared_ptr.

### `set_color_command`

Captures both border and fill so a single
command handles any color change.
shared_ptr keeps figure alive through delete/undo sequences.

### `set_background_color_command`

execute(): scene.set_background_color(new_color)
undo():    scene.set_background_color(old_color)

### `change_z_index_command`

execute(): figure->z_index = new_z, scene re-sorts figures.
undo():    figure->z_index = old_z, scene re-sorts figures.
shared_ptr keeps figure alive through delete/undo sequences.

### `clear_canvas_command`

execute(): snapshot = scene.get_all_figures()
          scene.clear_all_figures()
undo():    for each f in snapshot: scene.add_figure(f)
Re-snapshots on every execute() call, so redo is safe:
after undo() the scene is restored, and the next
execute() snapshots the same list and clears again.

