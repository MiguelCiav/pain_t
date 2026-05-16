#pragma once
#include "color.h"

class i_canvas {
public:
    virtual ~i_canvas() = default;
    virtual void put_pixel(int x, int y, const color& c) = 0;
    virtual void clear(const color& c) = 0;
    virtual int get_width() const = 0;
    virtual int get_height() const = 0;
};