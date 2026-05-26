#include "ellipse_tool.h"
#include "../engine/engine_2d.h"
#include "../figures/ellipse.h"
#include "../figures/figure.h"
#include "../figures/rectangle.h"
#include "../scene/app.h"
#include <string>

ellipse_tool::ellipse_tool(engine_2d *engine, app *application)
    : i_tool(engine, application) {}

void ellipse_tool::on_mouse_down(int button, point p) {
  is_drawing = true;
  starting_point = p;
  ending_point = p;
}

void ellipse_tool::on_mouse_move(point p) {
  if (!is_drawing) {
    return;
  }
  ending_point = p;
}

void ellipse_tool::on_mouse_up(int button, point p) {
  if (!is_drawing) {
    return;
  }
  is_drawing = false;
  ending_point = p;
  if (starting_point == ending_point) {
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

  figure *new_ellipse =
      new ellipse(starting_point, adjusted_end, application->get_border_color(),
                  application->get_fill_color(), true, engine);
  application->get_scene().add_figure(new_ellipse);
}

void ellipse_tool::on_key_down(int key) {}

void ellipse_tool::draw_preview() {
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

  rectangle bounding_box_rect(starting_point, adjusted_end, color(0.5f, 0.5f, 0.5f), color(0.0f, 0.0f, 0.0f), false, engine);
  bounding_box_rect.draw();

  ellipse temp_ellipse(starting_point, adjusted_end,
                       application->get_border_color(),
                       application->get_fill_color(), true, engine);
  temp_ellipse.draw();
}

std::string ellipse_tool::get_name() { return "ellipse_tool"; }
