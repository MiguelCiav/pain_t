#pragma once
#include "../engine/color.h"
#include "../engine/engine_2d.h"
#include "point.h"

namespace rasterizer {
// LINES
inline void draw_line_low(engine_2d *engine, point p1, point p2, color c) {
  int dx = p2.x - p1.x;
  int dy = p2.y - p1.y;
  int direction = (dy < 0 ? -1 : 1);
  dy = std::abs(dy);
  int D = (2 * dy) - dx;
  int y = p1.y;
  for (int x = p1.x; x < p2.x; x++) {
    engine->put_pixel(x, y, c);
    if (D > 0) {
      y += direction;
      D += 2 * (dy - dx);
    } else {
      D = D + 2 * dy;
    }
  }
}
inline void draw_line_high(engine_2d *engine, point p1, point p2, color c) {
  int dx = p2.x - p1.x;
  int dy = p2.y - p1.y;
  int direction = (dx < 0 ? -1 : 1);
  dx = std::abs(dx);
  int D = (2 * dx) - dy;
  int x = p1.x;
  for (int y = p1.y; y < p2.y; y++) {
    engine->put_pixel(x, y, c);
    if (D > 0) {
      x += direction;
      D += 2 * (dx - dy);
    } else {
      D = D + 2 * dx;
    }
  }
}
inline void draw_line(engine_2d *engine, point p1, point p2, color c) {
  if (std::abs(p2.y - p1.y) < std::abs(p2.x - p1.x)) {
    if (p1.x > p2.x)
      draw_line_low(engine, p2, p1, c);
    else
      draw_line_low(engine, p1, p2, c);
  } else {
    if (p1.y > p2.y)
      draw_line_high(engine, p2, p1, c);
    else
      draw_line_high(engine, p1, p2, c);
  }
}
inline void draw_horizontal_line(engine_2d *engine, int x1, int x2, int y,
                                 color c) {
  for (int x = x1; x1 <= x2; x++) {
    engine->put_pixel(x, y, c);
  }
}
inline void draw_vertical_line(engine_2d *engine, int y1, int y2, int x,
                               color c) {
  for (int y = y1; y <= y2; y++) {
    engine->put_pixel(x, y, c);
  }
}
// TRIANGLE FILLING
struct line_tracer_y {
  int x, y;
  int target_y;
  int dx, dy;
  int direction_x;
  int D;
  bool is_low;
  void init(point p1, point p2) {
    x = p1.x;
    y = p1.y;
    target_y = p2.y;
    dx = p2.x - p1.x;
    dy = p2.y - p1.y;
    direction_x = (dx < 0 ? -1 : 1);
    dx = std::abs(dx);
    is_low = (dx >= dy);
    if (is_low) {
      D = (2 * dy) - dx;
    } else {
      D = (2 * dx) - dy;
    }
  }
  void advance_to_next_y() {
    if (y >= target_y)
      return;
    if (!is_low) {
      // draw_line_high:
      if (D > 0) {
        x += direction_x;
        D += 2 * (dx - dy);
      } else {
        D += 2 * dx;
      }
      y++;
    } else {
      // draw_line_low:
      int current_y = y;
      while (y == current_y) {
        x += direction_x;
        if (D > 0) {
          y++;
          D += 2 * (dy - dx);
        } else {
          D += 2 * dy;
        }
      }
    }
  }
};

} // namespace rasterizer