#include "ellipse.h"
#include "../engine/engine_2d.h"
#include "bounding_box.h"
#include "control_point.h"
#include "figures/algebra.h"
#include "figures/figure.h"
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
    rasterizer::ellipse::draw_circle(engine, control_points[0].get_position(),
                                     a, border_color);
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
    rasterizer::ellipse::draw_circle(engine, control_points[0].get_position(),
                                     a, fill_color, true);
  } else {
    rasterizer::ellipse::draw(engine, control_points[0].get_position(), a, b,
                              fill_color, true);
  }
}

bool ellipse::on_border(point click) const {
  int a = std::abs(control_points[0].get_x() - control_points[2].get_x());
  int b = std::abs(control_points[0].get_y() - control_points[1].get_y());
  double result = algebra::ellipse::evaluate(click, this->get_center(), a, b);
  return std::abs(result - 1.0) <= ELLIPSE_TOLERANCE;
};

bool ellipse::on_filling(point click) const {
  int a = std::abs(control_points[0].get_x() - control_points[2].get_x());
  int b = std::abs(control_points[0].get_y() - control_points[1].get_y());
  double result = algebra::ellipse::evaluate(click, this->get_center(), a, b);
  return result <= 1.0;
};

bounding_box ellipse::get_bounding_box() {
  if (control_points.size() < 3) {
    throw std::logic_error("Ellipse must have at least 3 control points to compute bounding box");
  }
  point center = control_points[0].get_position();
  double a = std::abs(control_points[0].get_x() - control_points[2].get_x());
  double b = std::abs(control_points[0].get_y() - control_points[1].get_y());
  
  std::vector<point> corners = {
      point(center.x - a, center.y - b),
      point(center.x + a, center.y - b),
      point(center.x + a, center.y + b),
      point(center.x - a, center.y + b)
  };
  return bounding_box(corners);
}

std::string ellipse::get_type_tag() const { return "ellipse"; }

figure *ellipse::clone() const {
  std::vector<point> pts;
  for (const auto &cp : control_points) {
    pts.push_back(cp.get_position());
  }
  ellipse *cloned = new ellipse(pts, border_color, fill_color, filled, engine);
  cloned->set_bordered(bordered);
  cloned->set_z_index(z_index);
  return cloned;
}
