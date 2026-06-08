#include "../pain_t/src/engine/color.h"
#include "../pain_t/src/engine/engine_2d.h"
#include "../pain_t/src/scene/scene_serializer.h"
#include "../pain_t/src/figures/ellipse.h"
#include "../pain_t/src/figures/figure.h"
#include "../pain_t/src/figures/line.h"
#include "../pain_t/src/figures/point.h"
#include "../pain_t/src/figures/rectangle.h"
#include "../pain_t/src/figures/triangle.h"
#include "../pain_t/src/scene/app.h"
#include "../pain_t/src/scene/scene.h"
#include "../pain_t/src/commands/increase_degree_command.h"
#include "../pain_t/src/commands/move_figure_command.h"
#include "../pain_t/src/commands/deform_figure_command.h"
#include "../pain_t/src/commands/change_color_command.h"
#include "../pain_t/src/commands/reorder_figures_command.h"
#include "../pain_t/src/commands/clear_scene_command.h"
#include "../pain_t/src/commands/toggle_border_command.h"
#include "../pain_t/src/commands/toggle_fill_command.h"
#include "../pain_t/src/figures/bezier.h"
#include "../pain_t/src/tools/bezier_tool.h"
#include "../pain_t/src/tools/ellipse_tool.h"
#include "../pain_t/src/tools/line_tool.h"
#include "../pain_t/src/tools/rect_tool.h"
#include "../pain_t/src/tools/selection_tool.h"
#include "../pain_t/src/tools/triangle_tool.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

inline app &get_test_app() {
  static app test_app;
  static bool initialized = false;
  if (!initialized) {
    test_app.set_sidebar_width(0.0);
    test_app.setup();
    initialized = true;
  }
  // Clear figures and deselect on each query
  test_app.get_scene().deselect();
  test_app.get_scene().get_figures().clear();
  test_app.get_scene().init_tree(0.0, 0.0, test_app.get_width(), test_app.get_height());
  return test_app;
}

TEST_CASE("line_tool state logic and figure generation", "[tools][line_tool]") {
  app &test_app = get_test_app();
  line_tool tool(&test_app, &test_app);

  SECTION("tool identification") { REQUIRE(tool.get_name() == "line_tool"); }

  SECTION("mouse interaction workflow pushes new line") {
    REQUIRE(test_app.get_scene().get_figures().empty());

    // Click down at (10, 20)
    tool.on_mouse_down(0, point(10.0, 20.0));

    // Drag to (50, 60)
    tool.on_mouse_move(point(50.0, 60.0));

    // Release at (100, 120)
    tool.on_mouse_up(0, point(100.0, 120.0));

    REQUIRE(test_app.get_scene().get_figures().size() == 1);
    figure *created = test_app.get_scene().get_figures()[0];
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

  SECTION("key events have no side effects") { tool.on_key_down(42); }
}

// HELPER TO SIMULATE KEY PRESSES VIA GLFW CALLBACK
inline void simulate_glfw_key(GLFWwindow *win, int key, int action) {
  if (win) {
    GLFWkeyfun original_cb = glfwSetKeyCallback(win, nullptr);
    glfwSetKeyCallback(win, original_cb);
    if (original_cb) {
      original_cb(win, key, 0, action, 0);
    }
  }
}

TEST_CASE("rect_tool behavior and constraints", "[tools][rect_tool]") {
  app &test_app = get_test_app();
  rect_tool tool(&test_app, &test_app);
  GLFWwindow *win = glfwGetCurrentContext();

  SECTION("tool identification") { REQUIRE(tool.get_name() == "rect_tool"); }

  SECTION("mouse interaction pushes new rectangle") {
    REQUIRE(test_app.get_scene().get_figures().empty());
    tool.on_mouse_down(0, point(0.0, 0.0));
    tool.on_mouse_move(point(50.0, 60.0));
    tool.on_mouse_up(0, point(100.0, 50.0));

    REQUIRE(test_app.get_scene().get_figures().size() == 1);
    figure *created = test_app.get_scene().get_figures()[0];
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
    figure *created = test_app.get_scene().get_figures()[0];
    auto pts = created->get_control_points();

    // Height and width constrained to max(100, 50) = 100
    REQUIRE(pts[2].get_x() == 100.0);
    REQUIRE(pts[2].get_y() == 100.0);

    delete created;
  }
}

TEST_CASE("ellipse_tool behavior and constraints", "[tools][ellipse_tool]") {
  app &test_app = get_test_app();
  ellipse_tool tool(&test_app, &test_app);
  GLFWwindow *win = glfwGetCurrentContext();

  SECTION("tool identification") { REQUIRE(tool.get_name() == "ellipse_tool"); }

  SECTION("mouse interaction pushes new ellipse") {
    REQUIRE(test_app.get_scene().get_figures().empty());
    tool.on_mouse_down(0, point(0.0, 0.0));
    tool.on_mouse_move(point(50.0, 60.0));
    tool.on_mouse_up(0, point(100.0, 50.0));

    REQUIRE(test_app.get_scene().get_figures().size() == 1);
    figure *created = test_app.get_scene().get_figures()[0];
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
    figure *created = test_app.get_scene().get_figures()[0];
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
  app &test_app = get_test_app();
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
    figure *created = test_app.get_scene().get_figures()[0];
    REQUIRE(created->get_type_tag() == "triangle");
    REQUIRE(created->get_control_points().size() == 3);

    auto pts = created->get_control_points();
    REQUIRE(pts[0].get_x() == 0.0);
    REQUIRE(pts[1].get_x() == 100.0);
    REQUIRE(pts[2].get_y() == 100.0);

    delete created;
  }
}

TEST_CASE("bezier_tool multi-point drawing and finish trigger",
          "[tools][bezier_tool]") {
  app &test_app = get_test_app();
  bezier_tool tool(&test_app, &test_app);
  GLFWwindow *win = glfwGetCurrentContext();

  SECTION("tool identification") { REQUIRE(tool.get_name() == "bezier_tool"); }

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
    figure *created = test_app.get_scene().get_figures()[0];
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
  app &test_app = get_test_app();
  selection_tool tool(&test_app, &test_app);

  SECTION("overlapping z-order selection (select topmost)") {
    // Clear global scene
    test_app.get_scene().get_figures().clear();
    test_app.get_scene().deselect();

    // Create figure A: Rectangle from (0, 0) to (100, 100), filled
    figure *rectA = new rectangle(point(0, 0), point(100, 100), color(1, 0, 0),
                                  color(0, 1, 0), true, &test_app);
    test_app.get_scene().add_figure(rectA);

    // Create figure B: Rectangle from (50, 50) to (150, 150), filled
    figure *rectB =
        new rectangle(point(50, 50), point(150, 150), color(0, 0, 1),
                      color(1, 1, 0), true, &test_app);
    test_app.get_scene().add_figure(rectB);

    // Click at overlapping region (75, 75). rectB should be selected (z-order:
    // rectB is newer/topmost)
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

    // Create figure C: Rectangle from (0,0) to (100,100), NOT filled (border
    // only)
    figure *rectC = new rectangle(point(0, 0), point(100, 100), color(1, 0, 0),
                                  color(0, 1, 0), false, &test_app);
    test_app.get_scene().add_figure(rectC);

    // Click inside at (50, 50). Since it has no fill, inside() is false ->
    // should not select
    tool.on_mouse_down(0, point(50, 50));
    REQUIRE(test_app.get_scene().get_selected_figure() == nullptr);

    // Click on the border at (0, 50) -> should select
    tool.on_mouse_down(0, point(0, 50));
    REQUIRE(test_app.get_scene().get_selected_figure() == rectC);
  }

  SECTION("ellipse selection border vs filling") {
    test_app.get_scene().get_figures().clear();
    test_app.get_scene().deselect();

    // Create figure D: Ellipse from (0,0) to (100,100) (which constructs center
    // at 50,50, rx=50, ry=50), NOT filled
    figure *ellD = new ellipse(point(0, 0), point(100, 100), color(1, 0, 0),
                               color(0, 1, 0), false, &test_app);
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
  app &test_app = get_test_app();
  scene &sc = test_app.get_scene();

  REQUIRE(sc.get_figures().empty());
  REQUIRE(sc.get_selected_figure() == nullptr);

  figure *rectA = new rectangle(point(0, 0), point(100, 100), color(1, 0, 0),
                                color(0, 1, 0), true, &test_app);
  sc.add_figure(rectA);
  sc.select(rectA);

  REQUIRE(sc.get_figures().size() == 1);
  REQUIRE(sc.get_selected_figure() == rectA);

  sc.clear();

  REQUIRE(sc.get_figures().empty());
  REQUIRE(sc.get_selected_figure() == nullptr);
}

TEST_CASE("scene layers and z-index reordering", "[scene][layers]") {
  app &test_app = get_test_app();
  scene &sc = test_app.get_scene();

  figure *rectA = new rectangle(point(0, 0), point(100, 100), color(1, 0, 0),
                                color(0, 1, 0), true, &test_app);
  sc.add_figure(rectA);

  figure *rectB = new rectangle(point(50, 50), point(150, 150), color(0, 0, 1),
                                color(0, 1, 0), true, &test_app);
  sc.add_figure(rectB);

  REQUIRE(sc.get_figures().size() == 2);
  REQUIRE(sc.get_figures()[0] == rectA);
  REQUIRE(sc.get_figures()[1] == rectB);

  REQUIRE(sc.query(point(75, 75)) == rectB);

  sc.reorder_figures(1, 0);

  REQUIRE(sc.get_figures().size() == 2);
  REQUIRE(sc.get_figures()[0] == rectB);
  REQUIRE(sc.get_figures()[1] == rectA);

  REQUIRE(sc.query(point(75, 75)) == rectA);

  delete rectA;
  delete rectB;
}

TEST_CASE("selection_tool center cross dragging", "[tools][selection]") {
  app &test_app = get_test_app();
  selection_tool tool(&test_app, &test_app);
  scene &sc = test_app.get_scene();

  SECTION("dragging moves figure when clicking on center cross") {
    sc.get_figures().clear();
    sc.deselect();

    // Create a rectangle from (0, 0) to (100, 100), center is (50, 50)
    figure *rect = new rectangle(point(0, 0), point(100, 100), color(1, 0, 0),
                                 color(0, 1, 0), true, &test_app);
    sc.add_figure(rect);

    // Initial select
    tool.on_mouse_down(0, point(50, 50));
    REQUIRE(sc.get_selected_figure() == rect);

    // Click down exactly at center cross (50, 50)
    tool.on_mouse_down(0, point(50, 50));

    // Drag to (60, 70)
    tool.on_mouse_move(point(60, 70));

    // Release click
    tool.on_mouse_up(0, point(60, 70));

    // The center should have shifted by (10, 20)
    point new_center = rect->get_center();
    REQUIRE(new_center.x == 60.0);
    REQUIRE(new_center.y == 70.0);

    sc.deselect();
    delete rect;
  }

  SECTION("clicking outside cross tolerance does not drag figure") {
    sc.get_figures().clear();
    sc.deselect();

    // Create a rectangle from (0, 0) to (100, 100), center is (50, 50)
    figure *rect = new rectangle(point(0, 0), point(100, 100), color(1, 0, 0),
                                 color(0, 1, 0), true, &test_app);
    sc.add_figure(rect);

    // Initial select
    tool.on_mouse_down(0, point(50, 50));
    REQUIRE(sc.get_selected_figure() == rect);

    // Click down outside cross tolerance but inside the figure, e.g. at (20, 20)
    // Center is (50,50). Distance to (20,20) is sqrt(30^2 + 30^2) = 42.4 > tolerance (6)
    tool.on_mouse_down(0, point(20, 20));

    // Drag to (30, 40)
    tool.on_mouse_move(point(30, 40));

    tool.on_mouse_up(0, point(30, 40));

    // The figure center should NOT have shifted (it remained at 50, 50)
    point center = rect->get_center();
    REQUIRE(center.x == 50.0);
    REQUIRE(center.y == 50.0);

    sc.deselect();
    delete rect;
  }
}

TEST_CASE("selection_tool control point deformation", "[tools][selection]") {
  app &test_app = get_test_app();
  selection_tool tool(&test_app, &test_app);
  scene &sc = test_app.get_scene();

  SECTION("dragging a control point deforms the figure") {
    sc.get_figures().clear();
    sc.deselect();

    // Create a rectangle from (0, 0) to (100, 100).
    // Bounding box corners: pts[0] = (0,0), pts[1] = (100,0), pts[2] = (100,100), pts[3] = (0,100)
    figure *rect = new rectangle(point(0, 0), point(100, 100), color(1, 0, 0),
                                 color(0, 1, 0), true, &test_app);
    sc.add_figure(rect);

    // Initial select
    tool.on_mouse_down(0, point(50, 50));
    REQUIRE(sc.get_selected_figure() == rect);

    // Click down exactly at control point 0 (0, 0)
    tool.on_mouse_down(0, point(0, 0));

    // Drag to (15, -20)
    tool.on_mouse_move(point(15, -20));

    // Release click
    tool.on_mouse_up(0, point(15, -20));

    // Control point 0 should have shifted by (15, -20)
    point deformed_pt = rect->get_control_points()[0].get_position();
    REQUIRE(deformed_pt.x == 15.0);
    REQUIRE(deformed_pt.y == -20.0);

    // Control point 1 should remain unchanged at (100, 0)
    point unchanged_pt = rect->get_control_points()[1].get_position();
    REQUIRE(unchanged_pt.x == 100.0);
    REQUIRE(unchanged_pt.y == 0.0);

    sc.deselect();
    delete rect;
  }
}

TEST_CASE("selection_tool special figure handling", "[tools][selection]") {
  app &test_app = get_test_app();
  selection_tool tool(&test_app, &test_app);
  scene &sc = test_app.get_scene();

  SECTION("ellipse height and width handles are constrained and center cannot be deformed") {
    sc.get_figures().clear();
    sc.deselect();

    // Create an ellipse from (0, 0) to (100, 100).
    // Bounding Box center is (50, 50).
    // Control points: cp[0] = center (50, 50), cp[1] = height handle (50, 0), cp[2] = width handle (100, 50)
    figure *ell = new ellipse(point(0, 0), point(100, 100), color(1, 0, 0),
                              color(0, 1, 0), true, &test_app);
    sc.add_figure(ell);

    // Select ellipse
    tool.on_mouse_down(0, point(50, 50));
    REQUIRE(sc.get_selected_figure() == ell);

    // 1. Verify that clicking on center control point (50, 50) initiates center cross dragging, not deformation!
    tool.on_mouse_down(0, point(50, 50));
    tool.on_mouse_move(point(60, 70));
    tool.on_mouse_up(0, point(60, 70));

    // Verify it moved center and all points shifted (translation, center at 60, 70)
    REQUIRE(ell->get_control_points()[0].get_x() == 60.0);
    REQUIRE(ell->get_control_points()[0].get_y() == 70.0);

    // 2. Drag height handle (index 1), currently at (60, 20) since it shifted by (10, 20)
    REQUIRE(ell->get_control_points()[1].get_x() == 60.0);
    REQUIRE(ell->get_control_points()[1].get_y() == 20.0);

    // Click at height handle and drag to (75, 45). It should only update Y coordinate to 45, keeping X locked to center X (60)!
    tool.on_mouse_down(0, point(60, 20));
    tool.on_mouse_move(point(75, 45));
    tool.on_mouse_up(0, point(75, 45));

    REQUIRE(ell->get_control_points()[1].get_x() == 60.0); // locked to center X
    REQUIRE(ell->get_control_points()[1].get_y() == 45.0); // shifted vertically

    // 3. Drag width handle (index 2), currently at (110, 70) since it shifted by (10, 20)
    REQUIRE(ell->get_control_points()[2].get_x() == 110.0);
    REQUIRE(ell->get_control_points()[2].get_y() == 70.0);

    // Click at width handle and drag to (125, 95). It should only update X coordinate to 125, keeping Y locked to center Y (70)!
    tool.on_mouse_down(0, point(110, 70));
    tool.on_mouse_move(point(125, 95));
    tool.on_mouse_up(0, point(125, 95));

    REQUIRE(ell->get_control_points()[2].get_x() == 125.0); // shifted horizontally
    REQUIRE(ell->get_control_points()[2].get_y() == 70.0);  // locked to center Y

    sc.deselect();
    delete ell;
  }
}

TEST_CASE("scene save and load serialization", "[serialization]") {
  app &test_app = get_test_app();
  scene &sc = test_app.get_scene();
  sc.get_figures().clear();
  sc.deselect();
  sc.set_background_color(color(0.12f, 0.34f, 0.56f));

  // Add some figures to the scene
  figure *line1 = new ::line(point(10, 20), point(30, 40), color(1, 0, 0), &test_app);
  figure *rect1 = new rectangle(point(50, 60), point(150, 160), color(0, 1, 0), color(0, 0, 1), true, &test_app);
  sc.add_figure(line1);
  sc.add_figure(rect1);

  std::string filename = "test_serialization.p_t";
  
  SECTION("save scene to file") {
    REQUIRE(scene_serializer::save(sc, filename) == true);
  }

  SECTION("load scene from file") {
    // Make sure file exists by saving first
    REQUIRE(scene_serializer::save(sc, filename) == true);

    // Now clear the scene
    sc.clear();
    REQUIRE(sc.get_figures().empty());

    // Load it back
    REQUIRE(scene_serializer::load_into(filename, sc, &test_app) == true);

    // Verify background color
    REQUIRE(sc.get_background_color() == color(0.12f, 0.34f, 0.56f));

    // Verify figures count
    REQUIRE(sc.get_figures().size() == 2);

    // Verify first figure (line)
    figure *fig0 = sc.get_figures()[0];
    REQUIRE(fig0->get_type_tag() == "line");
    REQUIRE(fig0->get_border_color() == color(1, 0, 0));
    REQUIRE(fig0->get_control_points().size() == 2);
    REQUIRE(fig0->get_control_points()[0].get_position().x == 10.0);
    REQUIRE(fig0->get_control_points()[0].get_position().y == 20.0);
    REQUIRE(fig0->get_control_points()[1].get_position().x == 30.0);
    REQUIRE(fig0->get_control_points()[1].get_position().y == 40.0);

    // Verify second figure (rectangle)
    figure *fig1 = sc.get_figures()[1];
    REQUIRE(fig1->get_type_tag() == "rectangle");
    REQUIRE(fig1->get_border_color() == color(0, 1, 0));
    REQUIRE(fig1->get_fill_color() == color(0, 0, 1));
    REQUIRE(fig1->is_filled() == true);
    REQUIRE(fig1->get_control_points().size() == 4);
  }
}

TEST_CASE("increase_degree_command execution and undo/redo", "[commands]") {
  app &test_app = get_test_app();
  scene &sc = test_app.get_scene();
  sc.get_figures().clear();
  sc.deselect();

  std::vector<point> pts = {point(0, 0), point(50, 100), point(100, 0)};
  bezier *b = new bezier(pts, color(0, 0, 0), &test_app);
  sc.add_figure(b);

  increase_degree_command *cmd = new increase_degree_command(b, &sc);

  // Execute degree elevation
  sc.execute(cmd);
  REQUIRE(b->get_control_points().size() == 4);

  // Undo degree elevation
  sc.undo();
  REQUIRE(b->get_control_points().size() == 3);
  REQUIRE(b->get_control_points()[1].get_y() == 100.0);

  // Redo degree elevation
  sc.redo();
  REQUIRE(b->get_control_points().size() == 4);
}

TEST_CASE("new canvas commands execution and undo/redo", "[commands]") {
  app &test_app = get_test_app();
  scene &sc = test_app.get_scene();
  sc.get_figures().clear();
  sc.deselect();

  // Create two rectangle figures
  rectangle *rect1 = new rectangle(point(0, 0), point(10, 10), color(1, 0, 0), color(0, 1, 0), true, &test_app);
  rectangle *rect2 = new rectangle(point(20, 20), point(30, 30), color(0, 0, 1), color(1, 1, 0), true, &test_app);
  sc.add_figure(rect1);
  sc.add_figure(rect2);

  SECTION("move_figure_command") {
    point delta(5.0, -10.0);
    point old_pos = rect1->get_center();
    
    sc.execute(new move_figure_command(rect1, &sc, delta));
    REQUIRE(rect1->get_center().x == old_pos.x + 5.0);
    REQUIRE(rect1->get_center().y == old_pos.y - 10.0);

    sc.undo();
    REQUIRE(rect1->get_center().x == old_pos.x);
    REQUIRE(rect1->get_center().y == old_pos.y);

    sc.redo();
    REQUIRE(rect1->get_center().x == old_pos.x + 5.0);
    REQUIRE(rect1->get_center().y == old_pos.y - 10.0);
  }

  SECTION("deform_figure_command") {
    point old_cp = rect1->get_control_points()[1].get_position();
    point new_cp(15.0, 25.0);

    sc.execute(new deform_figure_command(rect1, &sc, 1, old_cp, new_cp));
    REQUIRE(rect1->get_control_points()[1].get_position().x == 15.0);
    REQUIRE(rect1->get_control_points()[1].get_position().y == 25.0);

    sc.undo();
    REQUIRE(rect1->get_control_points()[1].get_position().x == old_cp.x);
    REQUIRE(rect1->get_control_points()[1].get_position().y == old_cp.y);

    sc.redo();
    REQUIRE(rect1->get_control_points()[1].get_position().x == 15.0);
    REQUIRE(rect1->get_control_points()[1].get_position().y == 25.0);
  }

  SECTION("change_color_command") {
    color old_c = rect1->get_border_color();
    color new_c(0.2f, 0.4f, 0.6f);

    sc.execute(new change_color_command(rect1, color_type::border, old_c, new_c));
    REQUIRE(rect1->get_border_color() == new_c);

    sc.undo();
    REQUIRE(rect1->get_border_color() == old_c);

    sc.redo();
    REQUIRE(rect1->get_border_color() == new_c);
  }

  SECTION("reorder_figures_command") {
    REQUIRE(sc.get_figures()[0] == rect1);
    REQUIRE(sc.get_figures()[1] == rect2);

    sc.execute(new reorder_figures_command(&sc, 0, 1));
    REQUIRE(sc.get_figures()[0] == rect2);
    REQUIRE(sc.get_figures()[1] == rect1);

    sc.undo();
    REQUIRE(sc.get_figures()[0] == rect1);
    REQUIRE(sc.get_figures()[1] == rect2);

    sc.redo();
    REQUIRE(sc.get_figures()[0] == rect2);
    REQUIRE(sc.get_figures()[1] == rect1);
  }

  SECTION("clear_scene_command") {
    REQUIRE(sc.get_figures().size() == 2);

    sc.execute(new clear_scene_command(&sc));
    REQUIRE(sc.get_figures().empty());

    sc.undo();
    REQUIRE(sc.get_figures().size() == 2);
    REQUIRE(sc.get_figures()[0] == rect1);
    REQUIRE(sc.get_figures()[1] == rect2);

    sc.redo();
    REQUIRE(sc.get_figures().empty());
  }

  sc.deselect();
  sc.get_figures().clear();
  delete rect1;
  delete rect2;
}

TEST_CASE("active fill and border settings and toggles", "[scene][commands][tools]") {
  app &test_app = get_test_app();
  scene &sc = test_app.get_scene();

  SECTION("default active settings") {
    REQUIRE(sc.is_active_bordered() == true);
    REQUIRE(sc.is_active_filled() == true);

    sc.set_active_bordered(false);
    sc.set_active_filled(false);
    REQUIRE(sc.is_active_bordered() == false);
    REQUIRE(sc.is_active_filled() == false);

    // Reset
    sc.set_active_bordered(true);
    sc.set_active_filled(true);
  }

  SECTION("commands toggle border and fill") {
    rectangle *rect = new rectangle(point(0.0, 0.0), point(100.0, 100.0), color(0.0, 0.0, 0.0), color(1.0, 1.0, 1.0), true, nullptr);
    sc.add_figure(rect);
    sc.select(rect);

    REQUIRE(rect->is_bordered() == true);
    REQUIRE(rect->is_filled() == true);

    sc.execute(new toggle_border_command(rect, false));
    REQUIRE(rect->is_bordered() == false);

    sc.undo();
    REQUIRE(rect->is_bordered() == true);

    sc.redo();
    REQUIRE(rect->is_bordered() == false);

    sc.execute(new toggle_fill_command(rect, false));
    REQUIRE(rect->is_filled() == false);

    sc.undo();
    REQUIRE(rect->is_filled() == true);

    sc.redo();
    REQUIRE(rect->is_filled() == false);

    // Clean up
    sc.deselect();
    sc.get_figures().clear();
    delete rect;
  }

  SECTION("tools respect active settings when creating figures") {
    sc.set_active_bordered(false);
    sc.set_active_filled(false);

    rect_tool r_tool(&test_app, &test_app);
    r_tool.on_mouse_down(0, point(0.0, 0.0));
    r_tool.on_mouse_move(point(100.0, 100.0));
    r_tool.on_mouse_up(0, point(100.0, 100.0));

    REQUIRE(sc.get_figures().size() == 1);
    figure *rect = sc.get_figures()[0];
    REQUIRE(rect->is_bordered() == false);
    REQUIRE(rect->is_filled() == false);

    ellipse_tool e_tool(&test_app, &test_app);
    e_tool.on_mouse_down(0, point(0.0, 0.0));
    e_tool.on_mouse_move(point(100.0, 100.0));
    e_tool.on_mouse_up(0, point(100.0, 100.0));

    REQUIRE(sc.get_figures().size() == 2);
    figure *ell = sc.get_figures()[1];
    REQUIRE(ell->is_bordered() == false);
    REQUIRE(ell->is_filled() == false);

    // Reset settings
    sc.set_active_bordered(true);
    sc.set_active_filled(true);
    sc.get_figures().clear();
    delete rect;
    delete ell;
  }
}

TEST_CASE("tool icons check", "[tools][icons]") {
  app &test_app = get_test_app();
  
  selection_tool sel_t(&test_app, &test_app);
  line_tool l_t(&test_app, &test_app);
  rect_tool r_t(&test_app, &test_app);
  triangle_tool t_t(&test_app, &test_app);
  ellipse_tool e_t(&test_app, &test_app);
  bezier_tool b_t(&test_app, &test_app);

  REQUIRE(!sel_t.get_icon().empty());
  REQUIRE(!l_t.get_icon().empty());
  REQUIRE(!r_t.get_icon().empty());
  REQUIRE(!t_t.get_icon().empty());
  REQUIRE(!e_t.get_icon().empty());
  REQUIRE(!b_t.get_icon().empty());

  REQUIRE(sel_t.get_icon().find('/') != std::string::npos);
  REQUIRE(l_t.get_icon().find('/') != std::string::npos);
  REQUIRE(r_t.get_icon().find('|') != std::string::npos);
  REQUIRE(t_t.get_icon().find('/') != std::string::npos);
  REQUIRE(e_t.get_icon().find('(') != std::string::npos);
  REQUIRE(b_t.get_icon().find('\\') != std::string::npos);
}



