#include "rect_tool.h"
#include "../figures/figure.h"
#include "../figures/rectangle.h"
#include "../engine/engine_2d.h"
#include <string>

rect_tool::rect_tool(engine_2d* engine, std::vector<figure*>& figures)
    : engine(engine), scene_figures(figures) {}

void rect_tool::on_mouse_down(int button, point p) {
    is_drawing = true;
    starting_point = p;
    ending_point = p;
}

void rect_tool::on_mouse_move(point p) {
    if (is_drawing) {
        ending_point = p;
    }
}

void rect_tool::on_mouse_up(int button, point p) {
    if (is_drawing) {
        is_drawing = false;
        ending_point = p;
        
        std::vector<point> rect_points = {
            point(starting_point.x, starting_point.y),
            point(ending_point.x, starting_point.y),
            point(ending_point.x, ending_point.y),
            point(starting_point.x, ending_point.y)
        };
        
        figure* new_rect = new rectangle(rect_points, color(0, 0, 0), color(0, 0, 0), false, engine);
        scene_figures.push_back(new_rect);
    }
}

void rect_tool::on_key_down(int key) {}

void rect_tool::draw_preview() {
    if (is_drawing) {
        std::vector<point> rect_points = {
            point(starting_point.x, starting_point.y),
            point(ending_point.x, starting_point.y),
            point(ending_point.x, ending_point.y),
            point(starting_point.x, ending_point.y)
        };
        
        rectangle temp_rect(rect_points, color(0.6f, 0.6f, 0.6f), color(0, 0, 0), false, engine);
        temp_rect.draw_outline();
    }
}

std::string rect_tool::get_name() {
    return "rect_tool";
}
