#include "../engine/engine_2d.h"
#include "../figures/line.h"
#include <iostream>

class app : public engine_2d {
private:
    color background_color = color(0.9f, 0.9f, 0.9f);

public:
    app(): engine_2d(800, 600, "pain_t") {}

    void setup() override {
        clear(background_color);
        std::cout << "pain_t engine initialized successfully." << std::endl;
        
        // Draw a test line during initialization
        line l(point(100, 100), point(700, 500), color(0.1f, 0.1f, 0.1f), this);
        l.draw_outline();
    }

    // EVENTS

    void on_key_down(int key) override { }

    void on_mouse_button_down(int button, double x, double y) override { }

    void on_mouse_button_up(int button, double x, double y) override { }

    void on_mouse_move(double x, double y) override { }

    // UPDATES

    void update(float deltaTime) override { }

    void draw_ui() override { }
};

int main() {
    app engine;
    engine.run();
    return 0;
}
