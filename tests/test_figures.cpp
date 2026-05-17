#include <catch2/catch_test_macros.hpp>
#include "../pain_t/src/figures/point.h"
#include "../pain_t/src/figures/control_point.h"

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
