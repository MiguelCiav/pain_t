#pragma once

#include <string>
#include <vector>

#include "../engine/color.h"
#include "../engine/i_canvas.h"
#include "bounding_box.h"
#include "control_point.h"
#include "point.h"

class figure {
protected:
    int z_index;
    color border_color;
    color fill_color;
    bool is_selected;
    std::vector<control_point> control_points;
public:
    virtual ~figure() = default;
    virtual void draw(i_canvas*) = 0;
    virtual bool contains_point(double x, double y) const = 0;
    virtual bounding_box get_bounding_box() const = 0;
    virtual point get_center() const = 0;
    virtual std::vector<control_point> get_control_points() const = 0;
    virtual void translate(point delta) = 0;
    virtual bool has_fill() const = 0;
    virtual std::string get_type_tag() const = 0;
};