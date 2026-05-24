#include "line.h"
#include "../engine/engine_2d.h"
#include "rasterizer.h"
#include <stdexcept>

line::line(point p1, point p2, color border_color, engine_2d *engine)
    : figure(border_color, color(0, 0, 0), false, engine) {
  control_points.push_back(control_point(p1));
  control_points.push_back(control_point(p2));
}

bool line::can_fill() const { return false; }

void line::draw_fill() { throw std::logic_error("Cannot fill a line"); }

void line::draw_border() {
  if (control_points.size() != 2) {
    throw std::logic_error("Line must have 2 control points");
  }
  rasterizer::line::draw(engine, control_points[0].get_position(),
                         control_points[1].get_position(), border_color);
}

bool line::contains_point(double x, double y) const { return false; }

std::string line::get_type_tag() const { return "line"; }
