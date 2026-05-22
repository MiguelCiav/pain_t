#include "../engine/engine_2d.h"
#include "../figures/line.h"
#include "../tools/line_tool.h"
#include <iostream>
#include <vector>

class app : public engine_2d {
private:
    color background_color = color(0.85f, 0.85f, 0.85f);
    std::vector<figure*> figures;
    i_tool* active_tool = nullptr;

public:
    app(): engine_2d(800, 600, "pain_t") {}

    void setup() override {
        clear(background_color);
        std::cout << "pain_t engine initialized successfully." << std::endl;
        active_tool = new line_tool(this, figures);
    }

    // EVENTS

    void on_key_down(int key) override {
        if (active_tool) active_tool->on_key_down(key);
    }

    void on_mouse_button_down(int button, double x, double y) override {
        if (active_tool) active_tool->on_mouse_down(button, point(x, y));
    }

    void on_mouse_button_up(int button, double x, double y) override {
        if (active_tool) active_tool->on_mouse_up(button, point(x, y));
    }

    void on_mouse_move(double x, double y) override {
        if (active_tool) active_tool->on_mouse_move(point(x, y));
    }

    // UPDATES

    void update(float deltaTime) override {
        clear(background_color);
        
        for (figure* fig : figures) {
            fig->draw();
        }
        
        if (active_tool) {
            active_tool->draw_preview();
        }
    }

    void draw_ui() override { }
    
    ~app() {
        delete active_tool;
        for (figure* fig : figures) {
            delete fig;
        }
    }
};

int main() {
    app engine;
    engine.run();
    return 0;
}
