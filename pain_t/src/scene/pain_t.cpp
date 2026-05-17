#include "../engine/engine_2d.h"
#include <iostream>

class pain_t : public engine_2d {
private:
    color background_color = color(0.1f, 0.1f, 0.15f);
    color brush_color = color(1.0f, 0.0f, 0.0f);
    bool is_drawing = false;

public:
    pain_t(): engine_2d(1024, 600, "pain_t - A painful paint") {}

    void setup() override {
        clear(background_color);
        std::cout << "pain_t engine initialized successfully." << std::endl;
    }

    // EVENTS

    void on_key_down(int key) override {
        if (key == GLFW_KEY_SPACE) {
            clear(background_color);
        }
    }

    void on_mouse_button_down(int button, double x, double y) override {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            is_drawing = true;
            put_pixel(x, y, brush_color);
        }
    }

    void on_mouse_button_up(int button, double x, double y) override {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            is_drawing = false;
        }
    }

    void on_mouse_move(double x, double y) override {
        if (is_drawing) {
            int ix = static_cast<int>(x);
            int iy = static_cast<int>(y);
            put_pixel(ix, iy, brush_color);
        }
    }

    // UPDATES

    void update(float deltaTime) override {
        
    }

    void draw_ui() override {
        ImGui::Begin("Tools & Properties");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Separator();
        float col[3] = { brush_color.r, brush_color.g, brush_color.b };
        if (ImGui::ColorEdit3("Brush Color", col)) {
            brush_color.r = col[0];
            brush_color.g = col[1];
            brush_color.b = col[2];
        }
        ImGui::Separator();
        ImGui::Text("Instructions:");
        ImGui::BulletText("Hold LEFT CLICK to draw.");
        ImGui::BulletText("Press SPACE to clear canvas.");
        ImGui::End();
    }
};

int main() {
    pain_t app;
    app.run();
    return 0;
}
