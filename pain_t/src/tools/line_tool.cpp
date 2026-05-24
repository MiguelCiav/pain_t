#include "line_tool.h"
#include "../engine/engine_2d.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../scene/app.h"
#include <string>

line_tool::line_tool(engine_2d *engine, std::vector<figure *> &figures,
                     app *application)
    : i_tool(engine, figures, application) {}

void line_tool::on_mouse_down(int button, point p) {
  is_drawing = true;
  starting_point = p;
  ending_point = p;
}

void line_tool::on_mouse_move(point p) {
  if (!is_drawing) {
    return;
  }
  ending_point = p;
}

void line_tool::on_mouse_up(int button, point p) {
  if (!is_drawing) {
    return;
  }
  is_drawing = false;
  ending_point = p;
  figure *new_line = new line(starting_point, ending_point,
                              application->get_border_color(), engine);
  scene_figures.push_back(new_line);
}

void line_tool::on_key_down(int key) {}

void line_tool::draw_preview() {
  if (!is_drawing) {
    return;
  }
  line temp_line(starting_point, ending_point, application->get_border_color(),
                 engine);
  temp_line.draw_border();
}

std::string line_tool::get_name() { return "line_tool"; }
