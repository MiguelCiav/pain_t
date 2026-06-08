#pragma once
#include "../figures/point.h"
#include "i_tool.h"
#include <vector>

class engine_2d;
class figure;

class triangle_tool : public i_tool {
private:
  int state = 0;
  point p1, p2, p3;

public:
  triangle_tool(engine_2d *engine, app *application);

  void on_mouse_down(int button, point p) override;
  void on_mouse_move(point p) override;
  void on_mouse_up(int button, point p) override;
  void on_key_down(int key) override;
  void draw_preview() override;
  std::string get_name() override;
  std::string get_label() override { return "Triangle Tool"; }
  std::string get_icon() override {
    return "  /\\\n"
           " /  \\\n"
           "/____\\";
  }
};
