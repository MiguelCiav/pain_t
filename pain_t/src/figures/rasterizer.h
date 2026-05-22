#pragma once
#include <vector>
#include "../engine/color.h"
#include "../engine/engine_2d.h"
#include "point.h"

namespace rasterizer {
    void draw_line_low(engine_2d* engine, point p1, point p2, color c) {
        int dx = p2.x - p1.x;
        int dy = p2.y - p1.y;
        int direction = (dy < 0 ? -1 : 1);
        dy = std::abs(dy);
        int D = (2 * dy) - dx;
        int y = p1.y;
        for (int x = p1.x; x < p2.x; x++) {
            engine->put_pixel(x,y,c);
            if (D > 0) {
                y += direction;
                D += 2*(dy-dx);
            } else {
                D = D + 2*dy;
            }
        }
    }
    void draw_line_high(engine_2d* engine, point p1, point p2, color c) {
        int dx = p2.x - p1.x;
        int dy = p2.y - p1.y;
        int direction = (dx < 0 ? -1 : 1);
        dx = std::abs(dx);
        int D = (2 * dx) - dy;
        int x = p1.x;
        for (int y = p1.y; y < p2.y; y++) {
            engine->put_pixel(x,y,c);
            if (D > 0) {
                x += direction;
                D += 2*(dx-dy);
            } else {
                D = D + 2*dx;
            }
        }
    }
    void draw_line(engine_2d* engine, point p1, point p2, color c) {
        if (std::abs(p2.y - p1.y) < std::abs(p2.x - p1.x)) {
            if (p1.x > p2.x) draw_line_low(engine, p2, p1, c);
            else draw_line_low(engine, p1, p2, c);
        } else {
            if (p1.y > p2.y) draw_line_high(engine, p2, p1, c);
            else draw_line_high(engine, p1, p2, c);
        }
    }
}