#include "triangle.h"
#include "../engine/engine_2d.h"
#include "rasterizer.h"
#include <stdexcept>

triangle::triangle(color border_color, color fill_color, bool filled,
                   engine_2d *engine)
    : figure(border_color, color(0, 0, 0), filled, engine) {};

triangle::triangle(std::vector<point> points, color border_color,
                   color fill_color, bool filled, engine_2d *engine)
    : figure(border_color, color(0, 0, 0), filled, engine) {
  if (points.size() != 3)
    throw std::logic_error("A triangle must have 3 points");
  for (auto p : points)
    control_points.push_back(control_point(p));
};

void triangle::draw_border() {
  rasterizer::draw_line(engine, control_points[0].get_position(),
                        control_points[1].get_position(), border_color);
  rasterizer::draw_line(engine, control_points[1].get_position(),
                        control_points[2].get_position(), border_color);
  rasterizer::draw_line(engine, control_points[2].get_position(),
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

  rasterizer::line_tracer_y line_a;
  rasterizer::line_tracer_y line_b;

  line_a.init(p1, p3);
  line_b.init(p1, p2);

  for (int y = p1.y; y <= p3.y; y++) {
    int x_start = line_a.x;
    int x_end = line_b.x;

    if (x_start > x_end)
      std::swap(x_start, x_end);

    rasterizer::draw_horizontal_line(engine, x_start, x_end, y, fill_color);

    line_a.advance_to_next_y();
    line_b.advance_to_next_y();

    if (y == p2.y) {
      line_b.init(p2, p3);
    }
  }
}

bool triangle::contains_point(double x, double y) const { return false; }

std::string triangle::get_type_tag() const { return "triangle"; }
