#include "line_tool.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../engine/engine_2d.h"
#include "../scene/app.h"
#include <string>

line_tool::line_tool(engine_2d* engine, std::vector<figure*>& figures)
    : engine(engine), scene_figures(figures) {}

void line_tool::on_mouse_down(int button, point p) {
    is_drawing = true;
    starting_point = p;
    ending_point = p;
}

void line_tool::on_mouse_move(point p) {
    if (is_drawing) {
        ending_point = p;
    }
}

void line_tool::on_mouse_up(int button, point p) {
    if (is_drawing) {
        is_drawing = false;
        ending_point = p;
        color border_color = color(0, 0, 0);
        if (engine != nullptr) {
            app* application = static_cast<app*>(engine);
            border_color = application->get_border_color();
        }
        figure* new_line = new line(starting_point, ending_point, border_color, engine);
        scene_figures.push_back(new_line);
    }
}

void line_tool::on_key_down(int key) {}

void line_tool::draw_preview() {
    if (is_drawing) {
        color border_color = color(0, 0, 0);
        if (engine != nullptr) {
            app* application = static_cast<app*>(engine);
            border_color = application->get_border_color();
        }
        line temp_line(starting_point, ending_point, border_color, engine);
        temp_line.draw_border();
    }
}

std::string line_tool::get_name() {
    return "line_tool";
}
