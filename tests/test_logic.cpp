#include <catch2/catch_test_macros.hpp>
#include "engine_2d.h" 

TEST_CASE("color structure works", "[color]") {
    color c(1.0f, 0.5f, 0.0f);
    
    REQUIRE(c.r == 1.0f);
    REQUIRE(c.g == 0.5f);
    REQUIRE(c.b == 0.0f);
}

TEST_CASE("color default constructor", "[color]") {
    color c;
    
    REQUIRE(c.r == 0.0f);
    REQUIRE(c.g == 0.0f);
    REQUIRE(c.b == 0.0f);
}
