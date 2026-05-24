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
  figure *new_rect = new rectangle(starting_point, ending_point,
                                   application->get_border_color(),
                                   application->get_fill_color(), true, engine);
  scene_figures.push_back(new_rect);
}

void rect_tool::on_key_down(int key) {}

void rect_tool::draw_preview() {
  if (!is_drawing) {
    return;
  }
  rectangle temp_rect(starting_point, ending_point,
                      application->get_border_color(),
                      application->get_fill_color(), true, engine);
  temp_rect.draw();
}

std::string rect_tool::get_name() { return "rect_tool"; }
