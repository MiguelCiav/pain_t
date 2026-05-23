#include "rectangle.h"
#include "../engine/engine_2d.h"
#include "rasterizer.h"
#include <stdexcept>

rectangle::rectangle(color border_color, color fill_color, bool filled,
                     engine_2d *engine)
    : figure(border_color, color(0, 0, 0), filled, engine) {};

rectangle::rectangle(std::vector<point> points, color border_color,
                     color fill_color, bool filled, engine_2d *engine)
    : figure(border_color, color(0, 0, 0), filled, engine) {
  if (points.size() != 4)
    throw std::logic_error("A rectangle must have 4 points");
  for (auto p : points)
    control_points.push_back(control_point(p));
};

void rectangle::draw_outline() {
  rasterizer::draw_line(engine, control_points[0].get_position(),
                        control_points[1].get_position(), border_color);
  rasterizer::draw_line(engine, control_points[1].get_position(),
                        control_points[2].get_position(), border_color);
  rasterizer::draw_line(engine, control_points[2].get_position(),
                        control_points[3].get_position(), border_color);
  rasterizer::draw_line(engine, control_points[3].get_position(),
                        control_points[0].get_position(), border_color);
}

void rectangle::draw_fill() {
  if (filled) {
    double line_x1 = control_points[0].get_x();
    double line_x2 = control_points[1].get_x();
    double line_y1 = control_points[0].get_y();
    double line_y2 = control_points[2].get_y();
    for (double y = line_y1; y < line_y2; y++) {
      rasterizer::draw_line(engine, point{line_x1, y}, point{line_x2, y},
                            fill_color);
    }
  }
}

bool rectangle::contains_point(double x, double y) const {
  double line_x1 = control_points[0].get_x();
  double line_x2 = control_points[1].get_x();
  double line_y1 = control_points[0].get_y();
  double line_y2 = control_points[2].get_y();
  return x >= line_x1 && x <= line_x2 && y >= line_y1 && y <= line_y2;
}

bounding_box rectangle::get_bounding_box() const {
  double line_x1 = control_points[0].get_x();
  double line_x2 = control_points[1].get_x();
  double line_y1 = control_points[0].get_y();
  double line_y2 = control_points[2].get_y();
  return bounding_box{point{line_x1, line_y1}, point{line_x2, line_y2}};
}

std::string rectangle::get_type_tag() const { return "rectangle"; }
