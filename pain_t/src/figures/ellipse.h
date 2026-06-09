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
  bool on_border(point click) const override;
  bool on_filling(point click) const override;
  bounding_box get_bounding_box() override;
  point get_center() const override;
  std::string get_type_tag() const override;
  figure *clone() const override;
  void set_control_point(size_t index, point p) override;
};