#pragma once
#include "../engine/color.h"
#include "../engine/engine_2d.h"
#include "point.h"
#include <algorithm>
#include <stdexcept>

namespace rasterizer {
namespace line {
inline void draw_low(engine_2d *engine, point p1, point p2, color c) {
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
inline void draw_high(engine_2d *engine, point p1, point p2, color c) {
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
inline void draw(engine_2d *engine, point p1, point p2, color c) {
  if (std::abs(p2.y - p1.y) < std::abs(p2.x - p1.x)) {
    if (p1.x > p2.x)
      draw_low(engine, p2, p1, c);
    else
      draw_low(engine, p1, p2, c);
  } else {
    if (p1.y > p2.y)
      draw_high(engine, p2, p1, c);
    else
      draw_high(engine, p1, p2, c);
  }
}
inline void draw_horizontal(engine_2d *engine, int x1, int x2, int y, color c) {
  for (int x = x1; x <= x2; x++) {
    engine->put_pixel(x, y, c);
  }
}
inline void draw_vertical(engine_2d *engine, int y1, int y2, int x, color c) {
  for (int y = y1; y <= y2; y++) {
    engine->put_pixel(x, y, c);
  }
}
struct tracer_y {
  int x;
  int x_min, x_max;
  int y;
  int target_x, target_y;
  int dx, dy;
  int direction_x;
  int D;
  bool is_low;

  void compute_scanline_extent() {
    if (!is_low) {
      x_min = x_max = x;
      return;
    }
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
      if (D > 0) {
        x += direction_x;
        D += 2 * (dx - dy);
      } else {
        D += 2 * dx;
      }
      y++;
      x_min = x_max = x;
    } else {
      int current_y = y;
      while (y == current_y) {
        if (D > 0) {
          y++;
          D += 2 * (dy - dx);
        } else {
          D += 2 * dy;
        }
        if (y != current_y) {
          x += direction_x;
          break;
        }
        x += direction_x;
      }
      compute_scanline_extent();
    }
  }
};
} // namespace line
namespace ellipse {
inline void draw_8_points(engine_2d *engine, point center, int x, int y,
                          color c, bool fill = false) {
  if (fill) {
    line::draw_horizontal(engine, center.x - x, center.x + x, center.y + y, c);
    line::draw_horizontal(engine, center.x - x, center.x + x, center.y - y, c);
    line::draw_horizontal(engine, center.x - y, center.x + y, center.y + x, c);
    line::draw_horizontal(engine, center.x - y, center.x + y, center.y - x, c);
  } else {
    engine->put_pixel(center.x + x, center.y + y, c);
    engine->put_pixel(center.x - x, center.y + y, c);
    engine->put_pixel(center.x + x, center.y - y, c);
    engine->put_pixel(center.x - x, center.y - y, c);

    engine->put_pixel(center.x + y, center.y + x, c);
    engine->put_pixel(center.x - y, center.y + x, c);
    engine->put_pixel(center.x + y, center.y - x, c);
    engine->put_pixel(center.x - y, center.y - x, c);
  }
}

inline void draw_circle(engine_2d *engine, point center, int r, color c,
                        bool fill = false) {
  int x = 0;
  int y = r;
  int d = 1 - r;
  draw_8_points(engine, center, x, y, c, fill);
  while (y > x) {
    if (d < 0) {
      d += 2 * x + 3;
    } else {
      d += 2 * (x - y) + 5;
      y--;
    }
    x++;
    draw_8_points(engine, center, x, y, c, fill);
  }
}
inline void draw_ellipse_points(engine_2d *engine, point center, int x, int y,
                                color c, bool fill = false) {
  if (fill) {
    line::draw_horizontal(engine, center.x - x, center.x + x, center.y + y, c);
    line::draw_horizontal(engine, center.x - x, center.x + x, center.y - y, c);
  } else {
    engine->put_pixel(center.x + x, center.y + y, c);
    engine->put_pixel(center.x - x, center.y + y, c);
    engine->put_pixel(center.x + x, center.y - y, c);
    engine->put_pixel(center.x - x, center.y - y, c);
  }
}

inline void draw(engine_2d *engine, point center, int a, int b, color c,
                 bool fill = false) {
  if (a == 0 && b == 0) {
    engine->put_pixel(center.x, center.y, c);
    return;
  }
  if (b == 0) {
    line::draw_horizontal(engine, center.x - a, center.x + a, center.y, c);
    return;
  }
  if (a == 0) {
    line::draw_vertical(engine, center.y - b, center.y + b, center.x, c);
    return;
  }

  int x = 0;
  int y = b;

  int d = b * (4 * b - 4 * a * a) + a * a;

  draw_ellipse_points(engine, center, x, y, c, fill);

  while (b * b * 2 * (x + 1) < a * a * (2 * y - 1)) {
    if (d < 0) {
      d += 4 * (b * b * (2 * x + 3));
    } else {
      d += 4 * (b * b * (2 * x + 3) + a * a * (-2 * y + 2));
      y--;
    }
    x++;
    draw_ellipse_points(engine, center, x, y, c, fill);
  }

  d = b * b * (2 * x + 1) * (2 * x + 1) + 4 * a * a * (y - 1) * (y - 1) -
      4 * a * a * b * b;

  while (y > 0) {
    if (d < 0) {
      d += 4 * (b * b * (2 * x + 2) + a * a * (-2 * y + 3));
      x++;
    } else {
      d += 4 * a * a * (-2 * y + 3);
    }
    y--;
    draw_ellipse_points(engine, center, x, y, c, fill);
  }
}
} // namespace ellipse
namespace bezier {
inline const double bezier_step = 0.01;
inline void draw(engine_2d *engine, std::vector<point> points, color c) {
  if (points.size() <= 2)
    throw std::logic_error("Need more than two points to draw a bezier curve");
  point previous = point(0, 0);
  int steps = static_cast<int>(1.0 / bezier::bezier_step + 0.5);
  for (int step = 0; step <= steps; ++step) {
    double t = static_cast<double>(step) / steps;
    std::vector<point> aux_points = points;
    int curr_size = points.size();
    for (int i = 0; i < points.size() - 1; i++) {
      for (int j = 0; j < curr_size - 1; j++) {
        aux_points[j] = ((1 - t) * aux_points[j]) + (t * aux_points[j + 1]);
      }
      curr_size--;
    }
    if (step == 0) {
      previous = point(static_cast<int>(aux_points[0].x),
                       static_cast<int>(aux_points[0].y));
      engine->put_pixel(previous.x, previous.y, c);
      continue;
    }
    point current = point(static_cast<int>(aux_points[0].x),
                          static_cast<int>(aux_points[0].y));
    line::draw(engine, previous, current, c);
    previous = current;
  }
}
} // namespace bezier
} // namespace rasterizer