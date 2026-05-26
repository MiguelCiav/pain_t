#pragma once

#include "../engine/color.h"
#include "figure.h"
#include "point.h"
#include <string>

class engine_2d;

class bezier : public figure {
public:
  // CONSTRUCTORS
  bezier() = default;
  bezier(std::vector<point> points, color border_color, engine_2d *engine);

  // OVERRIDES
  bool can_fill() const override;
  void draw_fill() override;
  void draw_border() override;
  bool on_border(point click) const override;
  bool on_filling(point click) const override;
  std::string get_type_tag() const override;
};