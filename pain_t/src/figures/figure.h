#pragma once

#include "../engine/color.h"
#include "bounding_box.h"
#include "control_point.h"
#include "point.h"
#include <string>
#include <vector>

class engine_2d;

const int LINE_TOLERANCE = 3;
const double ELLIPSE_TOLERANCE = 0.08;

class figure {
protected:
  int z_index = 0;
  color border_color;
  color fill_color;
  bool selected = false;
  bool filled = false;
  bool bordered = true;
  engine_2d *engine = nullptr;
  std::vector<control_point> control_points;
  virtual bool on_border(point click) const = 0;
  virtual bool on_filling(point click) const = 0;

public:
  // CONSTRUCTORS AND DESTRUCTOR
  virtual ~figure() = default;
  figure() = default;
  figure(color border_color, color fill_color, bool filled,
         engine_2d *engine = nullptr);

  // GETTERS
  virtual int get_z_index() const { return z_index; }
  virtual color get_border_color() const { return border_color; }
  virtual color get_fill_color() const { return fill_color; }
  virtual bool is_selected() const { return selected; }
  virtual bool is_filled() const { return filled; }
  virtual bool is_bordered() const { return bordered; }
  virtual bool can_fill() const { return true; }
  virtual point get_center() const;
  virtual bounding_box get_bounding_box();
  virtual std::vector<control_point> &get_control_points() {
    return control_points;
  }
  virtual const std::vector<control_point> &get_control_points() const {
    return control_points;
  }
  virtual void set_control_point(size_t index, point p);

  // SETTERS
  virtual void set_engine(engine_2d *engine) { this->engine = engine; }
  virtual void set_z_index(int z_index) { this->z_index = z_index; }
  virtual void set_border_color(color border_color) {
    bordered = true;
    this->border_color = border_color;
  }
  virtual void set_fill_color(color fill_color) {
    if (can_fill()) {
      filled = true;
      this->fill_color = fill_color;
    }
  }
  virtual void set_bordered(bool bordered) { this->bordered = bordered; }
  virtual void set_filled(bool filled) {
    if (can_fill()) {
      this->filled = filled;
    }
  }
  virtual void select() { this->selected = true; }
  virtual void unselect() { this->selected = false; }

  // LOGIC
  virtual void move(point shift);
  virtual void draw();
  virtual void draw_border() = 0;
  virtual void draw_fill() = 0;
  virtual bool inside(point click) const;
  virtual std::string get_type_tag() const = 0;
  virtual figure *clone() const = 0;
  virtual void scale(double factor);
};