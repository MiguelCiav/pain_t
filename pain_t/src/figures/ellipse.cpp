#include "ellipse.h"
#include "../engine/engine_2d.h"
#include "figures/bounding_box.h"
#include "figures/control_point.h"
#include "rasterizer.h"
#include <stdexcept>
#include <vector>

ellipse::ellipse(color border_color, color fill_color, bool filled,
                 engine_2d *engine)
    : figure(border_color, fill_color, filled, engine) {};

ellipse::ellipse(std::vector<point> points, color border_color,
                 color fill_color, bool filled, engine_2d *engine)
    : figure(border_color, fill_color, filled, engine) {
  if (points.size() != 3)
    throw std::logic_error("A ellipse must have 3 points");
  for (auto p : points)
    control_points.push_back(control_point(p));
};

ellipse::ellipse(point center, point p1, point p2, color border_color,
                 color fill_color, bool filled, engine_2d *engine)
    : ellipse({center, p1, p2}, border_color, fill_color, filled, engine) {}

ellipse::ellipse(point p1, point p2, color border_color, color fill_color,
                 bool filled, engine_2d *engine)
    : figure(border_color, fill_color, filled, engine) {
  bounding_box bb = bounding_box({p1, p2});
  control_point center = control_point(bb.get_center());
  control_point a =
      control_point(point(bb.get_center().x, bb.get_bounding_box()[0].y));
  control_point b =
      control_point(point(bb.get_bounding_box()[1].x, bb.get_center().y));
  control_points = {center, a, b};
}

void ellipse::draw_border() {
  int a = std::abs(control_points[0].get_x() - control_points[2].get_x());
  int b = std::abs(control_points[0].get_y() - control_points[1].get_y());
  if (a == b) {
    rasterizer::ellipse::draw_circle(engine, control_points[0].get_position(), a,
                                     border_color);
  } else {
    rasterizer::ellipse::draw(engine, control_points[0].get_position(), a, b,
                              border_color);
  }
}

point ellipse::get_center() const { return control_points[0].get_position(); }

void ellipse::draw_fill() {
  if (!filled) {
    return;
  }
  int a = std::abs(control_points[0].get_x() - control_points[2].get_x());
  int b = std::abs(control_points[0].get_y() - control_points[1].get_y());
  if (a == b) {
    rasterizer::ellipse::draw_circle(engine, control_points[0].get_position(), a,
                                     fill_color, true);
  } else {
    rasterizer::ellipse::draw(engine, control_points[0].get_position(), a, b,
                              fill_color, true);
  }
}

bool ellipse::contains_point(double x, double y) const { return false; }

std::string ellipse::get_type_tag() const { return "ellipse"; }
