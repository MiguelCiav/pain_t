#pragma once

struct point {
    double x = 0.0;
    double y = 0.0;
    point() = default;
    point(double x, double y) : x(x), y(y) {}
};