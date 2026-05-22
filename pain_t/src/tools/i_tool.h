#pragma once
#include "../figures/point.h"
#include <string>

class i_tool {
public:
    virtual ~i_tool() = default;

    virtual void on_mouse_down(int button, point p) = 0;
    virtual void on_mouse_move(point p) = 0;
    virtual void on_mouse_up(int button, point p) = 0;
    virtual void on_key_down(int key) = 0;
    virtual void draw_preview() = 0;
    virtual std::string get_name() = 0;
};