#include "triangle.h"
#include "../engine/engine_2d.h"
#include "algebra.h"
#include "rasterizer.h"
#include <stdexcept>

triangle::triangle(color border_color, color fill_color, bool filled,
                   engine_2d *engine)
    : figure(border_color, fill_color, filled, engine) {};

triangle::triangle(std::vector<point> points, color border_color,
                   color fill_color, bool filled, engine_2d *engine)
    : figure(border_color, fill_color, filled, engine) {
  if (points.size() != 3)
    throw std::logic_error("A triangle must have 3 points");
  for (auto p : points)
    control_points.push_back(control_point(p));
};

void triangle::draw_border() {
  rasterizer::line::draw(engine, control_points[0].get_position(),
                         control_points[1].get_position(), border_color);
  rasterizer::line::draw(engine, control_points[1].get_position(),
                         control_points[2].get_position(), border_color);
  rasterizer::line::draw(engine, control_points[2].get_position(),
                         control_points[0].get_position(), border_color);
}

void triangle::draw_fill() {
  if (!filled) {
    return;
  }

  point p1 = control_points[0].get_position();
  point p2 = control_points[1].get_position();
  point p3 = control_points[2].get_position();

  if (p1.y > p2.y)
    std::swap(p1, p2);
  if (p1.y > p3.y)
    std::swap(p1, p3);
  if (p2.y > p3.y)
    std::swap(p2, p3);

  rasterizer::line::tracer_y line_a;
  rasterizer::line::tracer_y line_b;

  line_a.init(p1, p3);
  line_b.init(p1, p2);

  for (int y = p1.y; y <= p3.y; y++) {
    int x_left = std::min(line_a.x_min, line_b.x_min);
    int x_right = std::max(line_a.x_max, line_b.x_max);

    rasterizer::line::draw_horizontal(engine, x_left, x_right, y, fill_color);

    line_a.advance_to_next_y();
    line_b.advance_to_next_y();

    if (y == (int)p2.y) {
      line_b.init(p2, p3);
      line_b.advance_to_next_y();
    }
  }
}

bool triangle::on_border(point click) const {
  std::vector<double> distances;
  for (int i = 0; i <= 2; i++) {
    point a = control_points[i].get_position();
    point b = control_points[(i + 1) % 3].get_position();
    distances.push_back(algebra::line::distance_from_point(a, b, click));
  }
  auto min = std::min_element(distances.begin(), distances.end());
  return static_cast<int>(*min) < LINE_TOLERANCE;
};

bool triangle::on_filling(point click) const {
  std::vector<double> d;
  for (int i = 0; i <= 2; i++) {
    point a = control_points[i].get_position();
    point b = control_points[(i + 1) % 3].get_position();
    d.push_back(algebra::line::cross_product_2d(a, b, click));
  }
  bool has_neg = (d[0] <= 0) && (d[1] <= 0) && (d[2] <= 0);
  bool has_pos = (d[0] >= 0) && (d[1] >= 0) && (d[2] >= 0);
  return has_neg || has_pos;
};

std::string triangle::get_type_tag() const { return "triangle"; }
