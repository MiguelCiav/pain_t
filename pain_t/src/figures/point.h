#pragma once

struct point {
  double x = 0.0;
  double y = 0.0;
  point() = default;
  point(double x, double y) : x(x), y(y) {}
  point operator+(const point &other) const {
    return point(x + other.x, y + other.y);
  }
  point operator*(double scalar) const { return point(x * scalar, y * scalar); }
};

inline point operator*(double scalar, const point &p) {
  return point(p.x * scalar, p.y * scalar);
}