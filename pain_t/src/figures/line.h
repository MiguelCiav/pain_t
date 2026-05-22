#pragma once

#include "figure.h"
#include "rasterizer.h"
#include "../engine/engine_2d.h"
#include <stdexcept>
#include "bounding_box.h"
#include "point.h"
#include "control_point.h"
#include <string>

class line : public figure {
public:
    // CONSTRUCTORS
    line() = default;
    line(point p1, point p2, color border_color, engine_2d* engine = nullptr): 
        figure(border_color, color(0, 0, 0), false, engine) {
        control_points.push_back(control_point(p1));
        control_points.push_back(control_point(p2));
    }

    // OVERRIDES
    bool can_fill() const override { return false; }
    void draw_fill() override { throw std::logic_error("Cannot fill a line"); }
    void draw_outline() override {
        if (control_points.size() != 2) {
            throw std::logic_error("Line must have 2 control points");
        }
        rasterizer::draw_line(engine, control_points[0].get_position(), control_points[1].get_position(), border_color);
    }
    bool contains_point(double x, double y) const override { return false; }
    bounding_box get_bounding_box() const override {
        if (control_points.size() != 2) {
            throw std::logic_error("Line must have exactly 2 control points to calculate bounding box");
        }
        return bounding_box{control_points[0].get_position(), control_points[1].get_position()};
    }
    std::string get_type_tag() const override { return "line"; }
};