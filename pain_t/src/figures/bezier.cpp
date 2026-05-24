#include "bezier.h"
#include "../engine/engine_2d.h"
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

bool bezier::contains_point(double x, double y) const { return false; }

std::string bezier::get_type_tag() const { return "bezier"; }
