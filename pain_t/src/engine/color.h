#pragma once

struct color 
{
	float r, g, b;
	color() : r(0), g(0), b(0) {};
	color(float r, float g, float b) : r(r), g(g), b(b) {};
    bool operator==(const color& other) const {
        return r == other.r && g == other.g && b == other.b;
    }
};