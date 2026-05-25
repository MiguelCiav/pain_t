#include "rect_tool.h"
#include "../engine/engine_2d.h"
#include "../figures/figure.h"
#include "../figures/rectangle.h"
#include "../scene/app.h"
#include <string>

rect_tool::rect_tool(engine_2d *engine, std::vector<figure *> &figures,
                     app *application)
    : i_tool(engine, figures, application) {}

void rect_tool::on_mouse_down(int button, point p) {
  is_drawing = true;
  starting_point = p;
  ending_point = p;
}

void rect_tool::on_mouse_move(point p) {
  if (!is_drawing) {
    return;
  }
  if (engine == nullptr) {
    throw std::logic_error("Tool must have an engine");
  }
  ending_point = p;
}

void rect_tool::on_mouse_up(int button, point p) {
  if (!is_drawing) {
    return;
  }
  is_drawing = false;
  ending_point = p;
  
  point adjusted_end = ending_point;
  if (application->is_ctrl_pressed()) {
    int dx = ending_point.x - starting_point.x;
    int dy = ending_point.y - starting_point.y;
    int max_d = std::max(std::abs(dx), std::abs(dy));
    adjusted_end = point(starting_point.x + (dx < 0 ? -max_d : max_d),
                         starting_point.y + (dy < 0 ? -max_d : max_d));
  }

  figure *new_rect = new rectangle(starting_point, adjusted_end,
                                   application->get_border_color(),
                                   application->get_fill_color(), true, engine);
  scene_figures.push_back(new_rect);
}

void rect_tool::on_key_down(int key) {}

void rect_tool::draw_preview() {
  if (!is_drawing) {
    return;
  }

  point adjusted_end = ending_point;
  if (application->is_ctrl_pressed()) {
    int dx = ending_point.x - starting_point.x;
    int dy = ending_point.y - starting_point.y;
    int max_d = std::max(std::abs(dx), std::abs(dy));
    adjusted_end = point(starting_point.x + (dx < 0 ? -max_d : max_d),
                         starting_point.y + (dy < 0 ? -max_d : max_d));
  }

  rectangle temp_rect(starting_point, adjusted_end,
                      application->get_border_color(),
                      application->get_fill_color(), true, engine);
  temp_rect.draw();
}

std::string rect_tool::get_name() { return "rect_tool"; }
