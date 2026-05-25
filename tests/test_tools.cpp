#include <catch2/catch_test_macros.hpp>
#include "../pain_t/src/engine/color.h"
#include "../pain_t/src/figures/point.h"
#include "../pain_t/src/figures/figure.h"
#include "../pain_t/src/tools/line_tool.h"
#include "../pain_t/src/tools/rect_tool.h"
#include "../pain_t/src/tools/ellipse_tool.h"
#include "../pain_t/src/tools/triangle_tool.h"
#include "../pain_t/src/tools/bezier_tool.h"
#include "../pain_t/src/engine/engine_2d.h"
#include <vector>
#include "../pain_t/src/scene/app.h"

TEST_CASE("line_tool state logic and figure generation", "[tools][line_tool]") {
    std::vector<figure*> figures;
    app test_app;
    line_tool tool(&test_app, figures, &test_app);
    
    SECTION("tool identification") {
        REQUIRE(tool.get_name() == "line_tool");
    }
    
    SECTION("mouse interaction workflow pushes new line") {
        REQUIRE(figures.empty());
        
        // Click down at (10, 20)
        tool.on_mouse_down(0, point(10.0, 20.0));
        
        // Drag to (50, 60)
        tool.on_mouse_move(point(50.0, 60.0));
        
        // Release at (100, 120)
        tool.on_mouse_up(0, point(100.0, 120.0));
        
        REQUIRE(figures.size() == 1);
        figure* created = figures[0];
        REQUIRE(created->get_type_tag() == "line");
        REQUIRE(created->get_control_points().size() == 2);
        REQUIRE(created->get_control_points()[0].get_x() == 10.0);
        REQUIRE(created->get_control_points()[0].get_y() == 20.0);
        REQUIRE(created->get_control_points()[1].get_x() == 100.0);
        REQUIRE(created->get_control_points()[1].get_y() == 120.0);
        
        // Clean up
        delete created;
    }
    
    SECTION("on_mouse_up without on_mouse_down does nothing") {
        REQUIRE(figures.empty());
        tool.on_mouse_up(0, point(100.0, 120.0));
        REQUIRE(figures.empty());
    }
    
    SECTION("on_mouse_move without on_mouse_down does not change state") {
        tool.on_mouse_move(point(50.0, 60.0));
        tool.on_mouse_up(0, point(100.0, 120.0));
        REQUIRE(figures.empty());
    }
    
    SECTION("key events have no side effects") {
        tool.on_key_down(42);
    }
}

// HELPER TO SIMULATE KEY PRESSES VIA GLFW CALLBACK
inline void simulate_glfw_key(GLFWwindow* win, int key, int action) {
    if (win) {
        GLFWkeyfun original_cb = glfwSetKeyCallback(win, nullptr);
        glfwSetKeyCallback(win, original_cb);
        if (original_cb) {
            original_cb(win, key, 0, action, 0);
        }
    }
}

TEST_CASE("rect_tool behavior and constraints", "[tools][rect_tool]") {
    std::vector<figure*> figures;
    app test_app;
    rect_tool tool(&test_app, figures, &test_app);
    GLFWwindow* win = glfwGetCurrentContext();

    SECTION("tool identification") {
        REQUIRE(tool.get_name() == "rect_tool");
    }

    SECTION("mouse interaction pushes new rectangle") {
        REQUIRE(figures.empty());
        tool.on_mouse_down(0, point(0.0, 0.0));
        tool.on_mouse_move(point(50.0, 60.0));
        tool.on_mouse_up(0, point(100.0, 50.0));

        REQUIRE(figures.size() == 1);
        figure* created = figures[0];
        REQUIRE(created->get_type_tag() == "rectangle");
        REQUIRE(created->get_control_points().size() == 4);

        auto pts = created->get_control_points();
        REQUIRE(pts[0].get_x() == 0.0);
        REQUIRE(pts[0].get_y() == 0.0);
        REQUIRE(pts[2].get_x() == 100.0);
        REQUIRE(pts[2].get_y() == 50.0);

        delete created;
    }

    SECTION("aspect-ratio lock constraint with CTRL key") {
        REQUIRE(figures.empty());
        
        // Hold Left Ctrl
        simulate_glfw_key(win, GLFW_KEY_LEFT_CONTROL, GLFW_PRESS);
        
        tool.on_mouse_down(0, point(0.0, 0.0));
        tool.on_mouse_move(point(100.0, 50.0));
        tool.on_mouse_up(0, point(100.0, 50.0));
        
        // Release Left Ctrl
        simulate_glfw_key(win, GLFW_KEY_LEFT_CONTROL, GLFW_RELEASE);

        REQUIRE(figures.size() == 1);
        figure* created = figures[0];
        auto pts = created->get_control_points();
        
        // Height and width constrained to max(100, 50) = 100
        REQUIRE(pts[2].get_x() == 100.0);
        REQUIRE(pts[2].get_y() == 100.0);

        delete created;
    }
}

TEST_CASE("ellipse_tool behavior and constraints", "[tools][ellipse_tool]") {
    std::vector<figure*> figures;
    app test_app;
    ellipse_tool tool(&test_app, figures, &test_app);
    GLFWwindow* win = glfwGetCurrentContext();

    SECTION("tool identification") {
        REQUIRE(tool.get_name() == "ellipse_tool");
    }

    SECTION("mouse interaction pushes new ellipse") {
        REQUIRE(figures.empty());
        tool.on_mouse_down(0, point(0.0, 0.0));
        tool.on_mouse_move(point(50.0, 60.0));
        tool.on_mouse_up(0, point(100.0, 50.0));

        REQUIRE(figures.size() == 1);
        figure* created = figures[0];
        REQUIRE(created->get_type_tag() == "ellipse");
        REQUIRE(created->get_control_points().size() == 3);

        auto center = created->get_center();
        REQUIRE(center.x == 50.0);
        REQUIRE(center.y == 25.0);

        delete created;
    }

    SECTION("circle lock constraint with CTRL key") {
        REQUIRE(figures.empty());
        
        // Hold Right Ctrl
        simulate_glfw_key(win, GLFW_KEY_RIGHT_CONTROL, GLFW_PRESS);
        
        tool.on_mouse_down(0, point(0.0, 0.0));
        tool.on_mouse_move(point(100.0, 50.0));
        tool.on_mouse_up(0, point(100.0, 50.0));
        
        // Release Right Ctrl
        simulate_glfw_key(win, GLFW_KEY_RIGHT_CONTROL, GLFW_RELEASE);

        REQUIRE(figures.size() == 1);
        figure* created = figures[0];
        auto pts = created->get_control_points();
        
        // Center of square bb (0,0 to 100,100) -> (50,50)
        REQUIRE(pts[0].get_x() == 50.0);
        REQUIRE(pts[0].get_y() == 50.0);
        
        // y-radius handle -> (50, 0)
        REQUIRE(pts[1].get_x() == 50.0);
        REQUIRE(pts[1].get_y() == 0.0);

        delete created;
    }
}

TEST_CASE("triangle_tool 3-click FSM behavior", "[tools][triangle_tool]") {
    std::vector<figure*> figures;
    app test_app;
    triangle_tool tool(&test_app, figures, &test_app);

    SECTION("tool identification") {
        REQUIRE(tool.get_name() == "triangle_tool");
    }

    SECTION("multi-click sequence pushes new triangle") {
        REQUIRE(figures.empty());

        // First click (state 0 -> 1)
        tool.on_mouse_down(0, point(0.0, 0.0));
        tool.on_mouse_move(point(100.0, 0.0));
        REQUIRE(figures.empty());

        // Second click (state 1 -> 2)
        tool.on_mouse_down(0, point(100.0, 0.0));
        tool.on_mouse_move(point(50.0, 100.0));
        REQUIRE(figures.empty());

        // Third click commits (state 2 -> 0)
        tool.on_mouse_down(0, point(50.0, 100.0));
        
        REQUIRE(figures.size() == 1);
        figure* created = figures[0];
        REQUIRE(created->get_type_tag() == "triangle");
        REQUIRE(created->get_control_points().size() == 3);

        auto pts = created->get_control_points();
        REQUIRE(pts[0].get_x() == 0.0);
        REQUIRE(pts[1].get_x() == 100.0);
        REQUIRE(pts[2].get_y() == 100.0);

        delete created;
    }
}

TEST_CASE("bezier_tool multi-point drawing and finish trigger", "[tools][bezier_tool]") {
    std::vector<figure*> figures;
    app test_app;
    bezier_tool tool(&test_app, figures, &test_app);
    GLFWwindow* win = glfwGetCurrentContext();

    SECTION("tool identification") {
        REQUIRE(tool.get_name() == "bezier_tool");
    }

    SECTION("multi-click with enter trigger pushes bezier curve") {
        REQUIRE(figures.empty());

        // 1st click
        tool.on_mouse_down(0, point(0.0, 0.0));
        tool.on_mouse_up(0, point(0.0, 0.0));
        tool.on_mouse_move(point(50.0, 100.0));
        
        // 2nd click
        tool.on_mouse_down(0, point(50.0, 100.0));
        tool.on_mouse_up(0, point(50.0, 100.0));
        tool.on_mouse_move(point(100.0, 0.0));

        // 3rd click
        tool.on_mouse_down(0, point(100.0, 0.0));
        tool.on_mouse_up(0, point(100.0, 0.0));

        REQUIRE(figures.empty());

        // Press Enter to complete drawing
        simulate_glfw_key(win, GLFW_KEY_ENTER, GLFW_PRESS);
        tool.on_key_down(GLFW_KEY_ENTER);

        REQUIRE(figures.size() == 1);
        figure* created = figures[0];
        REQUIRE(created->get_type_tag() == "bezier");
        REQUIRE(created->get_control_points().size() == 3);

        auto pts = created->get_control_points();
        REQUIRE(pts[0].get_x() == 0.0);
        REQUIRE(pts[1].get_y() == 100.0);
        REQUIRE(pts[2].get_x() == 100.0);

        // Release Enter key
        simulate_glfw_key(win, GLFW_KEY_ENTER, GLFW_RELEASE);
        tool.on_key_down(GLFW_KEY_ENTER);

        delete created;
    }
}

