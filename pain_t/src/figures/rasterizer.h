#pragma once
#include "../engine/color.h"
#include "../engine/engine_2d.h"
#include "point.h"
#include <algorithm>

namespace rasterizer {
// LINES
inline void draw_line_low(engine_2d *engine, point p1, point p2, color c) {
  int dx = p2.x - p1.x;
  int dy = p2.y - p1.y;
  int direction = (dy < 0 ? -1 : 1);
  dy = std::abs(dy);
  int D = (2 * dy) - dx;
  int y = p1.y;
  for (int x = p1.x; x <= p2.x; x++) {
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
  for (int y = p1.y; y <= p2.y; y++) {
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
  for (int x = x1; x <= x2; x++) {
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
  int x;               // first pixel in walk direction (internal state)
  int x_min, x_max;    // leftmost and rightmost pixel at current scanline
  int y;
  int target_x, target_y;
  int dx, dy;
  int direction_x;
  int D;
  bool is_low;

  // Finds x_max for the current scanline by simulating forward with a
  // temporary copy of D. This does NOT modify the real D state.
  void compute_scanline_extent() {
    if (!is_low) {
      x_min = x_max = x;
      return;
    }
    // Simulate forward to find the last pixel at the current y
    int tmp_x = x;
    int tmp_D = D;
    while (true) {
      if (tmp_D > 0 || tmp_x == target_x) {
        break;
      }
      tmp_D += 2 * dy;
      tmp_x += direction_x;
    }
    x_min = std::min(x, tmp_x);
    x_max = std::max(x, tmp_x);
  }

  void init(point p1, point p2) {
    x = p1.x;
    y = p1.y;
    target_x = p2.x;
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
    compute_scanline_extent();
  }

  void advance_to_next_y() {
    if (y >= target_y)
      return;
    if (!is_low) {
      // Steep edge: one pixel per scanline, matches draw_line_high
      if (D > 0) {
        x += direction_x;
        D += 2 * (dx - dy);
      } else {
        D += 2 * dx;
      }
      y++;
      x_min = x_max = x;
    } else {
      // Shallow edge: walk through pixels at current y until y changes.
      // Process D BEFORE advancing x, matching draw_line_low's sequence:
      //   put_pixel(x, y) → check D → update D → increment x
      int current_y = y;
      while (y == current_y) {
        if (D > 0) {
          y++;
          D += 2 * (dy - dx);
        } else {
          D += 2 * dy;
        }
        if (y != current_y) {
          // y changed: current x was the last pixel of the old scanline.
          // The first pixel of the new scanline is at x + direction_x.
          x += direction_x;
          break;
        }
        x += direction_x;
      }
      compute_scanline_extent();
    }
  }
};

} // namespace rasterizer