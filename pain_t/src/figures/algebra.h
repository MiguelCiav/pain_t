#pragma once
#include "point.h"
#include <algorithm>
#include <cmath>

namespace algebra {
namespace line {
inline double distance_from_point(point a, point b, point c) {
  point vector_ab = b - a;
  point vector_ac = c - a;

  double magnitude = (vector_ab.x * vector_ab.x) + (vector_ab.y * vector_ab.y);

  if (magnitude == 0.0) {
    double dist_x = c.x - a.x;
    double dist_y = c.y - a.y;
    return std::sqrt((dist_x * dist_x) + (dist_y * dist_y));
  }

  double dot_product =
      (vector_ac.x * vector_ab.x) + (vector_ac.y * vector_ab.y);
  double t = dot_product / magnitude;

  t = std::max(0.0, std::min(1.0, t));

  point nearest_point = a + (vector_ab * t);

  double x_distance = c.x - nearest_point.x;
  double y_distance = c.y - nearest_point.y;

  return std::sqrt((x_distance * x_distance) + (y_distance * y_distance));
}
inline double cross_product_2d(point a, point b, point c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}
} // namespace line
namespace ellipse {
inline double evaluate(point click, point center, double a, double b) {
  if (a == 0.0 || b == 0.0) {
    return 999999.0;
  }
  double delta_x = click.x - center.x;
  double delta_y = click.y - center.y;
  double horizontal_term = (delta_x * delta_x) / (a * a);
  double vertical_term = (delta_y * delta_y) / (b * b);
  return horizontal_term + vertical_term;
}
} // namespace ellipse
} // namespace algebra