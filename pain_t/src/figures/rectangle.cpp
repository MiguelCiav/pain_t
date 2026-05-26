#include "rectangle.h"
#include "../engine/engine_2d.h"
#include "bounding_box.h"
#include "figures/algebra.h"
#include "rasterizer.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

rectangle::rectangle(color border_color, color fill_color, bool filled,
                     engine_2d *engine)
    : figure(border_color, fill_color, filled, engine) {};

rectangle::rectangle(std::vector<point> points, color border_color,
                     color fill_color, bool filled, engine_2d *engine)
    : figure(border_color, fill_color, filled, engine) {
  if (points.size() != 4)
    throw std::logic_error("A rectangle must have 4 points");
  for (auto p : points)
    control_points.push_back(control_point(p));
};

rectangle::rectangle(point p1, point p2, color border_color, color fill_color,
                     bool filled, engine_2d *engine)
    : figure(border_color, fill_color, filled, engine) {
  bounding_box bb = bounding_box({p1, p2});
  for (auto p : bb.get_bounding_box())
    control_points.push_back(control_point(p));
};

void rectangle::draw_border() {
  rasterizer::line::draw(engine, control_points[0].get_position(),
                         control_points[1].get_position(), border_color);
  rasterizer::line::draw(engine, control_points[1].get_position(),
                         control_points[2].get_position(), border_color);
  rasterizer::line::draw(engine, control_points[2].get_position(),
                         control_points[3].get_position(), border_color);
  rasterizer::line::draw(engine, control_points[3].get_position(),
                         control_points[0].get_position(), border_color);
}

void rectangle::draw_fill() {
  if (!filled) {
    return;
  }
  bounding_box bb = this->get_bounding_box();
  std::vector<point> points = bb.get_bounding_box();
  double line_x1 = points[0].x;
  double line_x2 = points[1].x;
  double line_y1 = points[0].y;
  double line_y2 = points[2].y;
  for (double y = line_y1; y < line_y2; y++) {
    rasterizer::line::draw(engine, point{line_x1, y}, point{line_x2, y},
                           fill_color);
  }
}

bool rectangle::on_border(point click) const {
  std::vector<double> distances;
  for (int i = 0; i <= 3; i++) {
    point a = control_points[i].get_position();
    point b = control_points[(i + 1) % 4].get_position();
    distances.push_back(algebra::line::distance_from_point(a, b, click));
  }
  auto min = std::min_element(distances.begin(), distances.end());
  return static_cast<int>(*min) < LINE_TOLERANCE;
};

bool rectangle::on_filling(point click) const {
  std::vector<double> d;
  for (int i = 0; i <= 3; i++) {
    point a = control_points[i].get_position();
    point b = control_points[(i + 1) % 4].get_position();
    d.push_back(algebra::line::cross_product_2d(a, b, click));
  }
  bool has_neg = (d[0] <= 0) && (d[1] <= 0) && (d[2] <= 0) && (d[3] <= 0);
  bool has_pos = (d[0] >= 0) && (d[1] >= 0) && (d[2] >= 0) && (d[3] >= 0);
  return has_neg || has_pos;
};

std::string rectangle::get_type_tag() const { return "rectangle"; }
