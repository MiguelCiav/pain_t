#include <catch2/catch_test_macros.hpp>
#include "engine2D.h" 

TEST_CASE("Color structure works", "[color]") {
    Color c(1.0f, 0.5f, 0.0f);
    
    REQUIRE(c.r == 1.0f);
    REQUIRE(c.g == 0.5f);
    REQUIRE(c.b == 0.0f);
}

TEST_CASE("Color default constructor", "[color]") {
    Color c;
    
    REQUIRE(c.r == 0.0f);
    REQUIRE(c.g == 0.0f);
    REQUIRE(c.b == 0.0f);
}
