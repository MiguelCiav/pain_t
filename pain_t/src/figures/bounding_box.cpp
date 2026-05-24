#include "bounding_box.h"
#include "point.h"
#include <limits>
#include <stdexcept>
#include <vector>

void bounding_box::set_points(std::vector<point> points) {
  if (points.empty() || points.size() <= 1)
    throw std::logic_error("A bounding box needs at least two points");
  double min_x, min_y;
  double max_x, max_y;
  min_x = min_y = std::numeric_limits<double>::max();
  max_x = max_y = std::numeric_limits<double>::min();
  for (auto p : points) {
    min_x = (min_x > p.x ? p.x : min_x);
    min_y = (min_y > p.y ? p.y : min_y);
    max_x = (max_x < p.x ? p.x : max_x);
    max_y = (max_y < p.y ? p.y : max_y);
  }
  this->points = {point(min_x, min_y), point(max_x, min_y), point(max_x, max_y),
                  point(min_x, max_y)};
}

void bounding_box::set_points(point p1, point p2, point p3, point p4) {
  set_points(std::vector<point>({p1, p2, p3, p4}));
}

bounding_box::bounding_box(std::vector<point> points) {
  bounding_box::set_points(points);
}

std::vector<point> bounding_box::get_bounding_box() { return points; }

point bounding_box::get_center() {
  if (points.empty() || points.size() <= 1)
    throw std::logic_error(
        "A bounding box needs at least two points to get the center");
  double x = 0, y = 0, n = 0;
  for (auto p : points) {
    x += p.x;
    y += p.y;
    n++;
  }
  return point(x / n, y / n);
}