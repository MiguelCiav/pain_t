#pragma once

#include <string>
#include <vector>
#include "../engine/color.h"
#include "control_point.h"
#include "bounding_box.h"
#include "point.h"

class engine_2d;

class figure {
protected:
    int z_index = 0;
    color border_color;
    color fill_color;
    bool selected = false;
    bool filled = false;
    engine_2d* engine = nullptr;
    std::vector<control_point> control_points;

public:
    // CONSTRUCTORS AND DESTRUCTOR
    virtual ~figure() = default;
    figure() = default;
    figure(color border_color, color fill_color, bool filled, engine_2d* engine = nullptr);

    // GETTERS
    virtual int get_z_index() const { return z_index; }
    virtual color get_border_color() const { return border_color; }
    virtual color get_fill_color() const { return fill_color; }
    virtual bool is_selected() const { return selected; }
    virtual bool is_filled() const { return filled; }
    virtual bool can_fill() const { return true; }
    virtual point get_center() const;
    virtual std::vector<control_point>& get_control_points() {
        return control_points;
    }
    virtual const std::vector<control_point>& get_control_points() const { 
        return control_points; 
    }

    // SETTERS
    virtual void set_engine(engine_2d* engine) { this->engine = engine; }
    virtual void set_z_index(int z_index) { this->z_index = z_index; }
    virtual void set_border_color(color border_color) { this->border_color = border_color; }
    virtual void set_fill_color(color fill_color) { this->fill_color = fill_color; }
    virtual void select() { this->selected = true; }
    virtual void unselect() { this->selected = false; }

    // LOGIC
    virtual void move(point shift);
    virtual void draw();
    virtual void draw_outline() = 0;
    virtual void draw_fill() = 0;
    virtual bool contains_point(double x, double y) const = 0;
    virtual bounding_box get_bounding_box() const = 0;
    virtual std::string get_type_tag() const = 0;
};