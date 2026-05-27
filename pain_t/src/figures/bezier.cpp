#include "bezier.h"
#include "../engine/engine_2d.h"
#include "figures/algebra.h"
#include "figures/control_point.h"
#include "figures/figure.h"
#include "rasterizer.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

bezier::bezier(std::vector<point> points, color border_color, engine_2d *engine)
    : figure(border_color, color(0, 0, 0), false, engine) {
  for (auto p : points) {
    control_points.push_back(control_point(p));
  }
}

bool bezier::can_fill() const { return false; }

void bezier::draw_fill() { throw std::logic_error("Cannot fill a bezier"); }

void bezier::draw_border() {
  if (control_points.size() < 3) {
    throw std::logic_error("bezier must have at least 3 control points");
  }
  std::vector<point> positions;
  positions.resize(control_points.size());
  std::transform(control_points.begin(), control_points.end(),
                 positions.begin(),
                 [](control_point cp) { return cp.get_position(); });
  rasterizer::bezier::draw(engine, positions, border_color);
}

std::vector<point> bezier::get_render_points() const {
  std::vector<point> result;
  std::vector<point> points;
  points.resize(control_points.size());
  std::transform(control_points.begin(), control_points.end(), points.begin(),
                 [](control_point p) { return p.get_position(); });

  int steps = static_cast<int>(1.0 / rasterizer::bezier::bezier_step + 0.5);
  for (int step = 0; step <= steps; ++step) {
    double t = static_cast<double>(step) / steps;
    std::vector<point> aux_points = points;
    int curr_size = points.size();
    for (int i = 0; i < points.size() - 1; i++) {
      for (int j = 0; j < curr_size - 1; j++) {
        aux_points[j] = ((1 - t) * aux_points[j]) + (t * aux_points[j + 1]);
      }
      curr_size--;
    }
    point current = point(static_cast<int>(aux_points[0].x),
                          static_cast<int>(aux_points[0].y));
    result.push_back(current);
  }

  return result;
}

bool bezier::on_border(point click) const {
  if (control_points.size() < 3) {
    throw std::logic_error(
        "cannot select a bezier with less than 3 control points");
  }

  std::vector<point> render_points = this->get_render_points();
  if (render_points.size() < 2)
    return false;

  for (size_t i = 0; i < render_points.size() - 1; ++i) {
    point a = render_points[i];
    point b = render_points[i + 1];

    double min_x = std::min(a.x, b.x) - BEZIER_BOX_TOLERANCE;
    double max_x = std::max(a.x, b.x) + BEZIER_BOX_TOLERANCE;
    double min_y = std::min(a.y, b.y) - BEZIER_BOX_TOLERANCE;
    double max_y = std::max(a.y, b.y) + BEZIER_BOX_TOLERANCE;

    if (click.x >= min_x && click.x <= max_x && click.y >= min_y &&
        click.y <= max_y) {
      if (algebra::line::distance_from_point(a, b, click) < LINE_TOLERANCE) {
        return true;
      }
    }
  }

  return false;
}

bool bezier::on_filling(point click) const { return false; }

std::string bezier::get_type_tag() const { return "bezier"; }
