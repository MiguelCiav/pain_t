#pragma once

#include "../engine/color.h"
#include "figure.h"
#include "point.h"
#include <string>

class engine_2d;

class ellipse : public figure {
public:
  // CONSTRUCTORS
  ellipse() = default;
  ellipse(color border_color, color fill_color, bool filled, engine_2d *engine);
  ellipse(std::vector<point> points, color border_color, color fill_color,
          bool filled, engine_2d *engine);
  ellipse(point center, point a, point b, color border_color, color fill_color,
          bool filled, engine_2d *engine);
  ellipse(point p1, point p2, color border_color, color fill_color, bool filled,
          engine_2d *engine);

  // OVERRIDES
  bool can_fill() const override { return true; };
  void draw_fill() override;
  void draw_border() override;
  bool contains_point(double x, double y) const override;
  point get_center() const override;
  std::string get_type_tag() const override;
};