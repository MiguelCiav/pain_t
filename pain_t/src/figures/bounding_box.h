#pragma once
#include "point.h"
#include <vector>

class bounding_box {
private:
  std::vector<point> points;

public:
  ~bounding_box() = default;
  bounding_box() = default;
  bounding_box(point p1, point p2, point p3, point p4)
      : points(std::vector<point>({p1, p2, p3, p4})) {}
  bounding_box(std::vector<point> points);
  std::vector<point> get_bounding_box();
  point get_center();
  void set_points(point p1, point p2, point p3, point p4);
  void set_points(std::vector<point> points);
};