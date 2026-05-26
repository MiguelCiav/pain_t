#include <catch2/catch_test_macros.hpp>
#include "../pain_t/src/engine/color.h"
#include "../pain_t/src/figures/point.h"
#include "../pain_t/src/figures/figure.h"
#include "../pain_t/src/figures/line.h"
#include "../pain_t/src/figures/rectangle.h"
#include "../pain_t/src/figures/triangle.h"
#include "../pain_t/src/figures/ellipse.h"
#include "../pain_t/src/tools/line_tool.h"
#include "../pain_t/src/tools/rect_tool.h"
#include "../pain_t/src/tools/ellipse_tool.h"
#include "../pain_t/src/tools/triangle_tool.h"
#include "../pain_t/src/tools/bezier_tool.h"
#include "../pain_t/src/engine/engine_2d.h"
#include <vector>
#include "../pain_t/src/scene/app.h"
#include "../pain_t/src/scene/scene.h"
#include "../pain_t/src/tools/selection_tool.h"

inline app& get_test_app() {
    static app test_app;
    // Clear figures and deselect on each query
    test_app.get_scene().deselect();
    test_app.get_scene().get_figures().clear();
    return test_app;
}

TEST_CASE("line_tool state logic and figure generation", "[tools][line_tool]") {
    app& test_app = get_test_app();
    line_tool tool(&test_app, &test_app);
    
    SECTION("tool identification") {
        REQUIRE(tool.get_name() == "line_tool");
    }
    
    SECTION("mouse interaction workflow pushes new line") {
        REQUIRE(test_app.get_scene().get_figures().empty());
        
        // Click down at (10, 20)
        tool.on_mouse_down(0, point(10.0, 20.0));
        
        // Drag to (50, 60)
        tool.on_mouse_move(point(50.0, 60.0));
        
        // Release at (100, 120)
        tool.on_mouse_up(0, point(100.0, 120.0));
        
        REQUIRE(test_app.get_scene().get_figures().size() == 1);
        figure* created = test_app.get_scene().get_figures()[0];
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
        REQUIRE(test_app.get_scene().get_figures().empty());
        tool.on_mouse_up(0, point(100.0, 120.0));
        REQUIRE(test_app.get_scene().get_figures().empty());
    }
    
    SECTION("on_mouse_move without on_mouse_down does not change state") {
        tool.on_mouse_move(point(50.0, 60.0));
        tool.on_mouse_up(0, point(100.0, 120.0));
        REQUIRE(test_app.get_scene().get_figures().empty());
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
    app& test_app = get_test_app();
    rect_tool tool(&test_app, &test_app);
    GLFWwindow* win = glfwGetCurrentContext();

    SECTION("tool identification") {
        REQUIRE(tool.get_name() == "rect_tool");
    }

    SECTION("mouse interaction pushes new rectangle") {
        REQUIRE(test_app.get_scene().get_figures().empty());
        tool.on_mouse_down(0, point(0.0, 0.0));
        tool.on_mouse_move(point(50.0, 60.0));
        tool.on_mouse_up(0, point(100.0, 50.0));

        REQUIRE(test_app.get_scene().get_figures().size() == 1);
        figure* created = test_app.get_scene().get_figures()[0];
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
        REQUIRE(test_app.get_scene().get_figures().empty());
        
        // Hold Left Ctrl
        simulate_glfw_key(win, GLFW_KEY_LEFT_CONTROL, GLFW_PRESS);
        
        tool.on_mouse_down(0, point(0.0, 0.0));
        tool.on_mouse_move(point(100.0, 50.0));
        tool.on_mouse_up(0, point(100.0, 50.0));
        
        // Release Left Ctrl
        simulate_glfw_key(win, GLFW_KEY_LEFT_CONTROL, GLFW_RELEASE);

        REQUIRE(test_app.get_scene().get_figures().size() == 1);
        figure* created = test_app.get_scene().get_figures()[0];
        auto pts = created->get_control_points();
        
        // Height and width constrained to max(100, 50) = 100
        REQUIRE(pts[2].get_x() == 100.0);
        REQUIRE(pts[2].get_y() == 100.0);

        delete created;
    }
}

TEST_CASE("ellipse_tool behavior and constraints", "[tools][ellipse_tool]") {
    app& test_app = get_test_app();
    ellipse_tool tool(&test_app, &test_app);
    GLFWwindow* win = glfwGetCurrentContext();

    SECTION("tool identification") {
        REQUIRE(tool.get_name() == "ellipse_tool");
    }

    SECTION("mouse interaction pushes new ellipse") {
        REQUIRE(test_app.get_scene().get_figures().empty());
        tool.on_mouse_down(0, point(0.0, 0.0));
        tool.on_mouse_move(point(50.0, 60.0));
        tool.on_mouse_up(0, point(100.0, 50.0));

        REQUIRE(test_app.get_scene().get_figures().size() == 1);
        figure* created = test_app.get_scene().get_figures()[0];
        REQUIRE(created->get_type_tag() == "ellipse");
        REQUIRE(created->get_control_points().size() == 3);

        auto center = created->get_center();
        REQUIRE(center.x == 50.0);
        REQUIRE(center.y == 25.0);

        delete created;
    }

    SECTION("circle lock constraint with CTRL key") {
        REQUIRE(test_app.get_scene().get_figures().empty());
        
        // Hold Right Ctrl
        simulate_glfw_key(win, GLFW_KEY_RIGHT_CONTROL, GLFW_PRESS);
        
        tool.on_mouse_down(0, point(0.0, 0.0));
        tool.on_mouse_move(point(100.0, 50.0));
        tool.on_mouse_up(0, point(100.0, 50.0));
        
        // Release Right Ctrl
        simulate_glfw_key(win, GLFW_KEY_RIGHT_CONTROL, GLFW_RELEASE);

        REQUIRE(test_app.get_scene().get_figures().size() == 1);
        figure* created = test_app.get_scene().get_figures()[0];
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
    app& test_app = get_test_app();
    triangle_tool tool(&test_app, &test_app);

    SECTION("tool identification") {
        REQUIRE(tool.get_name() == "triangle_tool");
    }

    SECTION("multi-click sequence pushes new triangle") {
        REQUIRE(test_app.get_scene().get_figures().empty());

        // First click (state 0 -> 1)
        tool.on_mouse_down(0, point(0.0, 0.0));
        tool.on_mouse_move(point(100.0, 0.0));
        REQUIRE(test_app.get_scene().get_figures().empty());

        // Second click (state 1 -> 2)
        tool.on_mouse_down(0, point(100.0, 0.0));
        tool.on_mouse_move(point(50.0, 100.0));
        REQUIRE(test_app.get_scene().get_figures().empty());

        // Third click commits (state 2 -> 0)
        tool.on_mouse_down(0, point(50.0, 100.0));
        
        REQUIRE(test_app.get_scene().get_figures().size() == 1);
        figure* created = test_app.get_scene().get_figures()[0];
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
    app& test_app = get_test_app();
    bezier_tool tool(&test_app, &test_app);
    GLFWwindow* win = glfwGetCurrentContext();

    SECTION("tool identification") {
        REQUIRE(tool.get_name() == "bezier_tool");
    }

    SECTION("multi-click with enter trigger pushes bezier curve") {
        REQUIRE(test_app.get_scene().get_figures().empty());

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

        REQUIRE(test_app.get_scene().get_figures().empty());

        // Press Enter to complete drawing
        simulate_glfw_key(win, GLFW_KEY_ENTER, GLFW_PRESS);
        tool.on_key_down(GLFW_KEY_ENTER);

        REQUIRE(test_app.get_scene().get_figures().size() == 1);
        figure* created = test_app.get_scene().get_figures()[0];
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

TEST_CASE("selection_tool complex scenarios and FSM", "[tools][selection]") {
    app& test_app = get_test_app();
    selection_tool tool(&test_app, &test_app);

    SECTION("overlapping z-order selection (select topmost)") {
        // Clear global scene
        test_app.get_scene().get_figures().clear();
        test_app.get_scene().deselect();

        // Create figure A: Rectangle from (0, 0) to (100, 100), filled
        figure* rectA = new rectangle(point(0,0), point(100,100), color(1,0,0), color(0,1,0), true, &test_app);
        test_app.get_scene().add_figure(rectA);

        // Create figure B: Rectangle from (50, 50) to (150, 150), filled
        figure* rectB = new rectangle(point(50,50), point(150,150), color(0,0,1), color(1,1,0), true, &test_app);
        test_app.get_scene().add_figure(rectB);

        // Click at overlapping region (75, 75). rectB should be selected (z-order: rectB is newer/topmost)
        tool.on_mouse_down(0, point(75, 75));
        REQUIRE(test_app.get_scene().get_selected_figure() == rectB);

        // Click at (25, 25). Only rectA contains it
        tool.on_mouse_down(0, point(25, 25));
        REQUIRE(test_app.get_scene().get_selected_figure() == rectA);

        // Click at (200, 200). Neither contains it -> deselects
        tool.on_mouse_down(0, point(200, 200));
        REQUIRE(test_app.get_scene().get_selected_figure() == nullptr);
    }

    SECTION("border vs filling selection (no fill)") {
        test_app.get_scene().get_figures().clear();
        test_app.get_scene().deselect();

        // Create figure C: Rectangle from (0,0) to (100,100), NOT filled (border only)
        figure* rectC = new rectangle(point(0,0), point(100,100), color(1,0,0), color(0,1,0), false, &test_app);
        test_app.get_scene().add_figure(rectC);

        // Click inside at (50, 50). Since it has no fill, inside() is false -> should not select
        tool.on_mouse_down(0, point(50, 50));
        REQUIRE(test_app.get_scene().get_selected_figure() == nullptr);

        // Click on the border at (0, 50) -> should select
        tool.on_mouse_down(0, point(0, 50));
        REQUIRE(test_app.get_scene().get_selected_figure() == rectC);
    }

    SECTION("ellipse selection border vs filling") {
        test_app.get_scene().get_figures().clear();
        test_app.get_scene().deselect();

        // Create figure D: Ellipse from (0,0) to (100,100) (which constructs center at 50,50, rx=50, ry=50), NOT filled
        figure* ellD = new ellipse(point(0,0), point(100,100), color(1,0,0), color(0,1,0), false, &test_app);
        test_app.get_scene().add_figure(ellD);

        // Click at center (50, 50) -> since not filled, should not select
        tool.on_mouse_down(0, point(50, 50));
        REQUIRE(test_app.get_scene().get_selected_figure() == nullptr);

        // Click on border at (50, 0) -> should select
        tool.on_mouse_down(0, point(50, 0));
        REQUIRE(test_app.get_scene().get_selected_figure() == ellD);
    }
}

TEST_CASE("scene clearing logic", "[scene]") {
    app& test_app = get_test_app();
    scene& sc = test_app.get_scene();

    REQUIRE(sc.get_figures().empty());
    REQUIRE(sc.get_selected_figure() == nullptr);

    figure* rectA = new rectangle(point(0,0), point(100,100), color(1,0,0), color(0,1,0), true, &test_app);
    sc.add_figure(rectA);
    sc.select(rectA);

    REQUIRE(sc.get_figures().size() == 1);
    REQUIRE(sc.get_selected_figure() == rectA);

    sc.clear();

    REQUIRE(sc.get_figures().empty());
    REQUIRE(sc.get_selected_figure() == nullptr);
}


