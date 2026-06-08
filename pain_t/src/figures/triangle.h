#pragma once

#include "../engine/color.h"
#include "figure.h"
#include "point.h"
#include <string>

class engine_2d;

class triangle : public figure {
public:
  // CONSTRUCTORS
  triangle() = default;
  triangle(color border_color, color fill_color, bool filled,
           engine_2d *engine);
  triangle(std::vector<point> points, color border_color, color fill_color,
           bool filled, engine_2d *engine);

  // OVERRIDES
  bool can_fill() const override { return true; };
  void draw_fill() override;
  void draw_border() override;
  bool on_border(point click) const override;
  bool on_filling(point click) const override;
  std::string get_type_tag() const override;
  figure *clone() const override;
};