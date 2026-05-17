#include "control_point.h"
#include <cmath>

bool control_point::contains_point(double x, double y, double tolerance) const {
    return std::abs(position.x - x) <= tolerance && std::abs(position.y - y) <= tolerance;
}

bool control_point::contains_point(point p, double tolerance) const {
    return std::abs(position.x - p.x) <= tolerance && std::abs(position.y - p.y) <= tolerance;
}