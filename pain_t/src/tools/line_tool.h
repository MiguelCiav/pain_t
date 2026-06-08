#pragma once
#include "../figures/point.h"
#include "i_tool.h"
#include <vector>

class engine_2d;
class figure;

class line_tool : public i_tool {
private:
  bool is_drawing = false;
  point starting_point, ending_point;

public:
  line_tool(engine_2d *engine, app *application);

  void on_mouse_down(int button, point p) override;
  void on_mouse_move(point p) override;
  void on_mouse_up(int button, point p) override;
  void on_key_down(int key) override;
  void draw_preview() override;
  std::string get_name() override;
  std::string get_label() override { return "Line Tool"; }
};