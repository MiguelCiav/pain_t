#pragma once
#include "i_tool.h"
#include "../figures/point.h"
#include <vector>

class engine_2d;
class figure;

class triangle_tool : public i_tool {
private:
    int state = 0; // 0: wait p1, 1: wait p2, 2: wait p3
    engine_2d* engine;
    point p1, p2, p3;
    std::vector<figure*>& scene_figures;

public:
    triangle_tool(engine_2d* engine, std::vector<figure*>& figures);
    
    void on_mouse_down(int button, point p) override;
    void on_mouse_move(point p) override;
    void on_mouse_up(int button, point p) override;
    void on_key_down(int key) override;
    void draw_preview() override;
    std::string get_name() override;
};
