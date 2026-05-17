#pragma once
#include "point.h"

class control_point {
private:
    point position;
public:
    // CONSTRUCTORS & DESTRUCTOR
    ~control_point() = default;
    control_point() = default;
    control_point(point p) : position(p) {}
    control_point(double x, double y) : position({x,y}) {}

    // GETTERS
    point get_position() const { return position; }
    double get_x() const { return position.x; }
    double get_y() const { return position.y; }

    // SETTERS
    void set_position(point position) { this->position = position; }
    void set_position(double x, double y) { this->position = point{x, y}; }
    void set_x(double x) { this->position.x = x; }
    void set_y(double y) { this->position.y = y; }

    // LOGIC
    bool contains_point(point p, double tolerance) const;
    bool contains_point(double x, double y, double tolerance) const;
};