#include "app.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../tools/line_tool.h"
#include <iostream>

app::app() : engine_2d(800, 600, "pain_t") {}

app::~app() {
    delete active_tool;
    for (figure* fig : figures) {
        delete fig;
    }
}

void app::setup() {
    clear(background_color);
    std::cout << "pain_t engine initialized successfully." << std::endl;
    active_tool = new line_tool(this, figures);
}

void app::on_key_down(int key) {
    if (active_tool) active_tool->on_key_down(key);
}

void app::on_mouse_button_down(int button, double x, double y) {
    if (active_tool) active_tool->on_mouse_down(button, point(x, y));
}

void app::on_mouse_button_up(int button, double x, double y) {
    if (active_tool) active_tool->on_mouse_up(button, point(x, y));
}

void app::on_mouse_move(double x, double y) {
    if (active_tool) active_tool->on_mouse_move(point(x, y));
}

void app::update(float deltaTime) {
    clear(background_color);
    
    for (figure* fig : figures) {
        fig->draw();
    }
    
    if (active_tool) {
        active_tool->draw_preview();
    }
}

void app::draw_ui() {}

int main() {
    app engine;
    engine.run();
    return 0;
}
