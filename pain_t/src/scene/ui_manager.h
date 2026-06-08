#pragma once

class app;

class ui_manager {
public:
  ui_manager() = default;
  ~ui_manager() = default;

  void render(app *application);

private:
  void draw_tool_selector(app *application);
  void draw_canvas_actions(app *application);
  void draw_file_operations(app *application);
  void draw_color_settings(app *application);
  void draw_layers_panel(app *application);
};
