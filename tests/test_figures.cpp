#include <catch2/catch_test_macros.hpp>
#include "../pain_t/src/figures/point.h"
#include "../pain_t/src/figures/control_point.h"
#include "../pain_t/src/figures/figure.h"

// DUMMY DATA

class dummy_figure : public figure {
public:
    void draw(i_canvas*) override {}
    bool contains_point(double x, double y) const override { return false; }
    bounding_box get_bounding_box() const override { return {}; }
    std::string get_type_tag() const override { return "dummy"; }
    void add_test_control_point(double x, double y) {
        control_points.push_back(control_point(point{x, y}));
    }
};

TEST_CASE("point storing and modifying", "[figures]") {
    SECTION("default initialization"){
        point p;
        REQUIRE(p.x == 0.0);
        REQUIRE(p.y == 0.0);
    }
    SECTION("parameterized initialization"){
        point p2(10.5, -3.2);
        REQUIRE(p2.x == 10.5);
        REQUIRE(p2.y == -3.2);
    }
    SECTION("can update coordinates directly"){
        point p;
        p.x = 10.5;
        p.y = -3.2;
        REQUIRE(p.x == 10.5);
        REQUIRE(p.y == -3.2);
    }
}
TEST_CASE("control_point behavioral checks", "[figures]") {
    control_point cp;
    SECTION("initialization and updates") {
        SECTION("default initialization") {
            REQUIRE(cp.get_position().x == 0.0);
            REQUIRE(cp.get_position().y == 0.0);
        }
        SECTION("can update position") {
            cp.set_position(10.5, -3.2);
            REQUIRE(cp.get_position().x == 10.5);
            REQUIRE(cp.get_position().y == -3.2);
        }
    }
    SECTION("hit detection") {
        control_point cp1(10.5, -3.2);
        SECTION("inside") {
            REQUIRE(cp1.contains_point(10.5, -3.2, 0.0));
            REQUIRE(cp1.contains_point(10.5, -3.2, 0.1));
        }
        SECTION("outside") {
            REQUIRE_FALSE(cp1.contains_point(10.5, -3.2, -0.1));
            REQUIRE_FALSE(cp1.contains_point(10.7, -3.2, 0.1));
        }
    }
}
TEST_CASE("figure base class behavior", "[figures]") {
    dummy_figure fig;
    SECTION("default values") {
        REQUIRE(fig.get_z_index() == 0);
        REQUIRE(fig.get_border_color() == (color{0, 0, 0}));
        REQUIRE(fig.get_fill_color() == (color{0, 0, 0}));
        REQUIRE(fig.get_control_points().empty());
        REQUIRE_FALSE(fig.is_selected());
        REQUIRE_FALSE(fig.is_filled());
    }
    SECTION("center of empty figure throws exception") {
        REQUIRE_THROWS_AS(fig.get_center(), std::logic_error);
    }
    SECTION("center of one point figure is the point itself") {
        fig.add_test_control_point(10.5, -3.2);
        REQUIRE(fig.get_center().x == 10.5);
        REQUIRE(fig.get_center().y == -3.2);
    }
    SECTION("center of multiple points is the centroid") {
        fig.add_test_control_point(0.0, 0.0);
        fig.add_test_control_point(10.0, 0.0);
        fig.add_test_control_point(10.0, 10.0);
        fig.add_test_control_point(0.0, 10.0);
        REQUIRE(fig.get_center().x == 5.0);
        REQUIRE(fig.get_center().y == 5.0);
    }
    SECTION("moving an empty figure throws exception") {
        REQUIRE_THROWS_AS(fig.move({1.0, 1.0}), std::logic_error);
    }
    SECTION("moving a figure shifts all control points by the same amount") {
        fig.add_test_control_point(0.0, 0.0);
        fig.add_test_control_point(10.0, 0.0);
        fig.add_test_control_point(10.0, 10.0);
        fig.add_test_control_point(0.0, 10.0);
        fig.move({1.0, 2.0});
        REQUIRE(fig.get_control_points()[0].get_x() == 1.0);
        REQUIRE(fig.get_control_points()[0].get_y() == 2.0);
        REQUIRE(fig.get_control_points()[1].get_x() == 11.0);
        REQUIRE(fig.get_control_points()[1].get_y() == 2.0);
        REQUIRE(fig.get_control_points()[2].get_x() == 11.0);
        REQUIRE(fig.get_control_points()[2].get_y() == 12.0);
        REQUIRE(fig.get_control_points()[3].get_x() == 1.0);
        REQUIRE(fig.get_control_points()[3].get_y() == 12.0);
    }
}
