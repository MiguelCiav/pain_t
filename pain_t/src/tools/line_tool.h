#pragma once
#include "i_tool.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../engine/engine_2d.h"
#include "../figures/point.h"
#include <vector>
#include <string>

class line_tool : public i_tool {
private:
    bool is_drawing = false;
    engine_2d* engine;
    point starting_point, ending_point;
    std::vector<figure*>& scene_figures;

public:
    line_tool(engine_2d* engine, std::vector<figure*>& figures) 
        : engine(engine), scene_figures(figures) {}
    
    void on_mouse_down(int button, point p) override {
        is_drawing = true;
        starting_point = p;
        ending_point = p;
    }

    void on_mouse_move(point p) override {
        if(is_drawing) {
            ending_point = p;
        }
    }

    void on_mouse_up(int button, point p) override {
        if(is_drawing) {
            is_drawing = false;
            ending_point = p;
            figure* new_line = new line(starting_point, ending_point, color(0, 0, 0), engine);
            scene_figures.push_back(new_line);
        }
    }

    void on_key_down(int key) override {}

    void draw_preview() override {
        if(is_drawing) {
            line temp_line(starting_point, ending_point, color(0.6f, 0.6f, 0.6f), engine);
            temp_line.draw_outline();
        }
    }

    std::string get_name() override {
        return "line_tool";
    }
};