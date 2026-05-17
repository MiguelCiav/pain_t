#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include "../engine/color.h"
#include "../engine/i_canvas.h"
#include "control_point.h"
#include "bounding_box.h"
#include "point.h"

class figure {
protected:
    int z_index = 0;
    color border_color;
    color fill_color;
    bool selected = false;
    bool filled = false;
    std::vector<control_point> control_points;

public:
    // CONSTRUCTORS AND DESTRUCTOR
    virtual ~figure() = default;

    // GETTERS
    virtual int get_z_index() const { return z_index; }
    virtual color get_border_color() const { return border_color; }
    virtual color get_fill_color() const { return fill_color; }
    virtual bool is_selected() const { return selected; }
    virtual bool is_filled() const { return filled; }
    virtual point get_center() const {
        if (control_points.empty()) {
            throw std::logic_error("Cannot get center of an empty figure");
        }
        if (control_points.size() == 1) {
            return control_points[0].get_position();
        }
        point center = {0.0, 0.0};
        for (auto& cp : control_points) {
            center.x += cp.get_x();
            center.y += cp.get_y();
        }
        center.x /= control_points.size();
        center.y /= control_points.size();
        return center;
    }
    virtual std::vector<control_point>& get_control_points() {
        return control_points;
    }
    virtual const std::vector<control_point>& get_control_points() const { 
        return control_points; 
    }

    // SETTERS
    virtual void set_z_index(int z_index) { this->z_index = z_index; }
    virtual void set_border_color(color border_color) { this->border_color = border_color; }
    virtual void set_fill_color(color fill_color) { this->fill_color = fill_color; }
    virtual void select() { this->selected = true; }
    virtual void unselect() { this->selected = false; }

    // LOGIC
    virtual void move(point shift) {
        if (control_points.empty()) {
            throw std::logic_error("Cannot move an empty figure");
        }
        for (auto& cp : control_points) {
            cp.set_x(cp.get_x() + shift.x);
            cp.set_y(cp.get_y() + shift.y);
        }
    }
    virtual void draw(i_canvas*) = 0;
    virtual bool contains_point(double x, double y) const = 0;
    virtual bounding_box get_bounding_box() const = 0;
    virtual std::string get_type_tag() const = 0;
};