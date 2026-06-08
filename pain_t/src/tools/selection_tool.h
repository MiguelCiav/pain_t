#pragma once
#include "../figures/point.h"
#include "i_tool.h"

class engine_2d;
class figure;
class app;

class selection_tool : public i_tool {
public:
  selection_tool(engine_2d *engine, app *application);

  void on_mouse_down(int button, point p) override;
  void on_mouse_move(point p) override;
  void on_mouse_up(int button, point p) override;
  void on_key_down(int key) override;
  void draw_preview() override;
  void draw_settings() override;
  std::string get_name() override;
  std::string get_label() override { return "Selection Tool"; }

private:
  bool is_dragging = false;
  point last_mouse_point;
  point cumulative_shift;
  bool is_deforming = false;
  int active_control_point_idx = -1;
  point deform_start_pos;

  bool try_select_control_point(figure *selected, point p);
  bool try_select_center(figure *selected, point p);
  void query_new_figure(point p);
  void deform_figure(figure *selected, point p);
  void drag_figure(figure *selected, point p);

  void draw_bounding_box(figure *selected);
  void draw_selection_cross(figure *selected);
  void draw_bezier_guidelines(figure *selected);
  void draw_control_points(figure *selected);
};
