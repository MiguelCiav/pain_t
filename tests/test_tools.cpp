#include <catch2/catch_test_macros.hpp>
#include "../pain_t/src/engine/color.h"
#include "../pain_t/src/figures/point.h"
#include "../pain_t/src/figures/figure.h"
#include "../pain_t/src/tools/line_tool.h"
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
