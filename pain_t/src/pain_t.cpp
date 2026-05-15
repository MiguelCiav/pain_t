#include "engine2D.h"
#include <iostream>

class pain_t : public Engine2D {
private:
    Color background_color = Color(0.1f, 0.1f, 0.15f);
    Color brush_color = Color(1.0f, 0.0f, 0.0f);
    bool is_drawing = false;

public:
    pain_t(): Engine2D(1024, 600, "pain_t - A painful paint") {}

    void setup() override {
        clear(background_color);
        std::cout << "pain_t engine initialized successfully." << std::endl;
    }

    // EVENTS

    void onkeyDown(int key) override {
        if (key == GLFW_KEY_SPACE) {
            clear(background_color);
        }
    }

    void onMouseButtonDown(int button, double x, double y) override {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            is_drawing = true;
            putPixel(x, y, brush_color);
        }
    }

    void onMouseButtonUp(int button, double x, double y) override {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            is_drawing = false;
        }
    }

    void onMouseMove(double x, double y) override {
        if (is_drawing) {
            int ix = static_cast<int>(x);
            int iy = static_cast<int>(y);
            putPixel(ix, iy, brush_color);
        }
    }

    // UPDATES

    void update(float deltaTime) override {
        
    }

    void drawUI() override {
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
