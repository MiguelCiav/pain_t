#include "figure.h"
#include "../engine/engine_2d.h"
#include <algorithm>
#include <stdexcept>

figure::figure(color border_color, color fill_color, bool filled,
               engine_2d *engine)
    : border_color(border_color), fill_color(fill_color), filled(filled),
      engine(engine) {}

point figure::get_center() const {
  if (control_points.empty()) {
    throw std::logic_error("Cannot get center of an empty figure");
  }
  if (control_points.size() == 1) {
    return control_points[0].get_position();
  }
  point center = {0.0, 0.0};
  for (auto &cp : control_points) {
    center.x += cp.get_x();
    center.y += cp.get_y();
  }
  center.x /= control_points.size();
  center.y /= control_points.size();
  return center;
}

void figure::move(point shift) {
  if (control_points.empty()) {
    throw std::logic_error("Cannot move an empty figure");
  }
  for (auto &cp : control_points) {
    cp.set_x(cp.get_x() + shift.x);
    cp.set_y(cp.get_y() + shift.y);
  }
}

void figure::draw() {
  if (engine == nullptr) {
    throw std::logic_error("Cannot draw a figure without an engine");
  }
  if (filled) {
    draw_fill();
  }
  draw_border();
}

bounding_box figure::get_bounding_box() {
  if (control_points.size() <= 1)
    throw std::logic_error(
        "Cannot get the bounding box of a figure with less than 2 points");
  std::vector<point> points;
  points.resize(control_points.size());
  std::transform(control_points.begin(), control_points.end(), points.begin(),
                 [](control_point cp) { return cp.get_position(); });
  return bounding_box(points);
}
