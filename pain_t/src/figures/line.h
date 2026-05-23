#pragma once

#include "../engine/color.h"
#include "figure.h"
#include "point.h"
#include <string>

class engine_2d;

class line : public figure {
public:
  // CONSTRUCTORS
  line() = default;
  line(point p1, point p2, color border_color, engine_2d *engine = nullptr);

  // OVERRIDES
  bool can_fill() const override;
  void draw_fill() override;
  void draw_outline() override;
  bool contains_point(double x, double y) const override;
  std::string get_type_tag() const override;
};