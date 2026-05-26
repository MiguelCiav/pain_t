#include <catch2/catch_test_macros.hpp>
#include "../pain_t/src/engine/color.h"
#include "../pain_t/src/figures/point.h"
#include "../pain_t/src/figures/bounding_box.h"
#include "../pain_t/src/figures/control_point.h"
#include "../pain_t/src/figures/figure.h"
#include "../pain_t/src/figures/line.h"
#include "../pain_t/src/figures/rectangle.h"
#include "../pain_t/src/figures/triangle.h"
#include "../pain_t/src/figures/ellipse.h"
#include "../pain_t/src/figures/bezier.h"
#include "../pain_t/src/engine/engine_2d.h"
#include <vector>
#include <stdexcept>

// DUMMY FIGURE FOR BASE CLASS TESTING
class dummy_figure : public figure {
public:
    dummy_figure() = default;
    dummy_figure(color border_color, color fill_color, bool filled, engine_2d* engine = nullptr)
        : figure(border_color, fill_color, filled, engine) {}

    void draw() override {}
    void draw_border() override {}
    void draw_fill() override {}
    bool on_border(point click) const override { return false; }
    bool on_filling(point click) const override { return false; }
    bounding_box get_bounding_box() override { return {}; }
    std::string get_type_tag() const override { return "dummy"; }
    void add_test_control_point(double x, double y) {
        control_points.push_back(control_point(point{x, y}));
    }
};

// DUMMY TO TEST INTERACTION WITH FIGURE::DRAW()
class draw_test_figure : public figure {
public:
    draw_test_figure() = default;
    draw_test_figure(color border_color, color fill_color, bool filled, engine_2d* engine = nullptr)
        : figure(border_color, fill_color, filled, engine) {}
    
    bool draw_border_called = false;
    bool draw_fill_called = false;
    
    void draw_border() override { draw_border_called = true; }
    void draw_fill() override { draw_fill_called = true; }
    bool on_border(point click) const override { return false; }
    bool on_filling(point click) const override { return false; }
    bounding_box get_bounding_box() override { return {}; }
    std::string get_type_tag() const override { return "draw_test"; }
};

TEST_CASE("color struct coverage", "[color]") {
    SECTION("default constructor") {
        color c;
        REQUIRE(c.r == 0.0f);
        REQUIRE(c.g == 0.0f);
        REQUIRE(c.b == 0.0f);
    }
    SECTION("parameterized constructor") {
        color c(0.1f, 0.5f, 0.9f);
        REQUIRE(c.r == 0.1f);
        REQUIRE(c.g == 0.5f);
        REQUIRE(c.b == 0.9f);
    }
    SECTION("equality operators") {
        color c1(0.2f, 0.3f, 0.4f);
        color c2(0.2f, 0.3f, 0.4f);
        color c3(0.5f, 0.3f, 0.4f);
        color c4(0.2f, 0.9f, 0.4f);
        color c5(0.2f, 0.3f, 0.8f);
        
        REQUIRE(c1 == c2);
        REQUIRE_FALSE(c1 == c3);
        REQUIRE_FALSE(c1 == c4);
        REQUIRE_FALSE(c1 == c5);
    }
}

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

TEST_CASE("bounding_box struct coverage", "[bounding_box]") {
    point min_pt(1.0, 2.0);
    point max_pt(3.0, 4.0);
    std::vector<point> pts = {min_pt, max_pt};
    bounding_box box{pts};
    
    auto box_pts = box.get_bounding_box();
    REQUIRE(box_pts.size() == 4);
    REQUIRE(box_pts[0].x == 1.0);
    REQUIRE(box_pts[0].y == 2.0);
    REQUIRE(box_pts[1].x == 3.0);
    REQUIRE(box_pts[1].y == 2.0);
    REQUIRE(box_pts[2].x == 3.0);
    REQUIRE(box_pts[2].y == 4.0);
    REQUIRE(box_pts[3].x == 1.0);
    REQUIRE(box_pts[3].y == 4.0);
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
        SECTION("can update single coordinate") {
            cp.set_x(12.0);
            cp.set_y(-5.0);
            REQUIRE(cp.get_x() == 12.0);
            REQUIRE(cp.get_y() == -5.0);
        }
        SECTION("set position with point") {
            point p(4.0, 8.0);
            cp.set_position(p);
            REQUIRE(cp.get_x() == 4.0);
            REQUIRE(cp.get_y() == 8.0);
        }
    }
    SECTION("hit detection") {
        control_point cp1(10.5, -3.2);
        SECTION("inside") {
            REQUIRE(cp1.contains_point(10.5, -3.2, 0.0));
            REQUIRE(cp1.contains_point(10.5, -3.2, 0.1));
            
            point p_exact(10.5, -3.2);
            REQUIRE(cp1.contains_point(p_exact, 0.0));
        }
        SECTION("outside") {
            REQUIRE_FALSE(cp1.contains_point(10.5, -3.2, -0.1));
            REQUIRE_FALSE(cp1.contains_point(10.7, -3.2, 0.1));
            
            point p_out(10.7, -3.2);
            REQUIRE_FALSE(cp1.contains_point(p_out, 0.1));
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
    SECTION("parameterized constructor values") {
        color border(1.0f, 0.0f, 0.0f);
        color fill(0.0f, 0.0f, 1.0f);
        dummy_figure fig_param(border, fill, true, nullptr);
        REQUIRE(fig_param.get_border_color() == border);
        REQUIRE(fig_param.get_fill_color() == fill);
        REQUIRE(fig_param.is_filled());
    }
    SECTION("getters and setters and basic modifiers") {
        fig.set_z_index(42);
        REQUIRE(fig.get_z_index() == 42);
        
        color border(0.1f, 0.2f, 0.3f);
        color fill(0.4f, 0.5f, 0.6f);
        fig.set_border_color(border);
        fig.set_fill_color(fill);
        REQUIRE(fig.get_border_color() == border);
        REQUIRE(fig.get_fill_color() == fill);
        
        fig.select();
        REQUIRE(fig.is_selected());
        fig.unselect();
        REQUIRE_FALSE(fig.is_selected());
        
        REQUIRE(fig.can_fill());
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
    SECTION("const control points getter") {
        fig.add_test_control_point(1.0, 2.0);
        const dummy_figure& const_fig = fig;
        REQUIRE(const_fig.get_control_points().size() == 1);
        REQUIRE(const_fig.get_control_points()[0].get_x() == 1.0);
    }
    SECTION("draw throws logic_error if engine is nullptr") {
        draw_test_figure df;
        df.set_engine(nullptr);
        REQUIRE_THROWS_AS(df.draw(), std::logic_error);
    }
}

TEST_CASE("line primitive coverage", "[figures][line]") {
    point p1(10.0, 20.0);
    point p2(100.0, 200.0);
    color line_color(0.5f, 0.5f, 0.5f);
    
    SECTION("default constructor") {
        line l;
        REQUIRE(l.get_control_points().empty());
        REQUIRE(l.get_type_tag() == "line");
    }
    
    SECTION("parameterized constructor") {
        line l(p1, p2, line_color, nullptr);
        REQUIRE(l.get_control_points().size() == 2);
        REQUIRE(l.get_control_points()[0].get_x() == 10.0);
        REQUIRE(l.get_control_points()[0].get_y() == 20.0);
        REQUIRE(l.get_control_points()[1].get_x() == 100.0);
        REQUIRE(l.get_control_points()[1].get_y() == 200.0);
        REQUIRE(l.get_border_color() == line_color);
        REQUIRE(l.get_type_tag() == "line");
        REQUIRE_FALSE(l.can_fill());
    }
    
    SECTION("draw_fill throws logic_error") {
        line l(p1, p2, line_color, nullptr);
        REQUIRE_THROWS_AS(l.draw_fill(), std::logic_error);
    }
    
    SECTION("draw_border exceptions") {
        line l(p1, p2, line_color, nullptr);
        
        // draw() throws logic_error before calling draw_border if engine is nullptr
        REQUIRE_THROWS_AS(l.draw(), std::logic_error);
        
        // Verify draw_border throws if control points are not exactly 2
        l.get_control_points().clear();
        REQUIRE_THROWS_AS(l.draw_border(), std::logic_error);
    }
    
    SECTION("inside hit testing") {
        line l(p1, p2, line_color, nullptr);
        REQUIRE(l.inside(point(10.0, 20.0)));
        REQUIRE_FALSE(l.inside(point(50.0, 110.0)));
    }
    
    SECTION("get_bounding_box behavior") {
        line l(p1, p2, line_color, nullptr);
        bounding_box box = l.get_bounding_box();
        auto box_pts = box.get_bounding_box();
        REQUIRE(box_pts.size() == 4);
        REQUIRE(box_pts[0].x == 10.0);
        REQUIRE(box_pts[0].y == 20.0);
        REQUIRE(box_pts[1].x == 100.0);
        REQUIRE(box_pts[1].y == 20.0);
        REQUIRE(box_pts[2].x == 100.0);
        REQUIRE(box_pts[2].y == 200.0);
        REQUIRE(box_pts[3].x == 10.0);
        REQUIRE(box_pts[3].y == 200.0);
        
        // If control points are not 2, should throw
        l.get_control_points().clear();
        REQUIRE_THROWS_AS(l.get_bounding_box(), std::logic_error);
    }
}

TEST_CASE("rectangle primitive coverage", "[figures][rectangle]") {
    point p1(0.0, 0.0);
    point p2(100.0, 50.0);
    color border_color(1.0f, 0.0f, 0.0f);
    color fill_color(0.0f, 1.0f, 0.0f);

    SECTION("default constructor") {
        rectangle r;
        REQUIRE(r.get_control_points().empty());
        REQUIRE(r.get_type_tag() == "rectangle");
        REQUIRE(r.can_fill());
    }

    SECTION("parameterized constructor (p1, p2)") {
        rectangle r(p1, p2, border_color, fill_color, true, nullptr);
        REQUIRE(r.get_control_points().size() == 4);
        REQUIRE(r.get_border_color() == border_color);
        REQUIRE(r.get_fill_color() == fill_color);
        REQUIRE(r.is_filled());
        
        // Corners generated from p1(0,0) and p2(100,50)
        // bounding box should be (0,0), (100,0), (100,50), (0,50)
        auto pts = r.get_control_points();
        REQUIRE(pts[0].get_x() == 0.0);
        REQUIRE(pts[0].get_y() == 0.0);
        REQUIRE(pts[1].get_x() == 100.0);
        REQUIRE(pts[1].get_y() == 0.0);
        REQUIRE(pts[2].get_x() == 100.0);
        REQUIRE(pts[2].get_y() == 50.0);
        REQUIRE(pts[3].get_x() == 0.0);
        REQUIRE(pts[3].get_y() == 50.0);
    }

    SECTION("parameterized constructor (vector points)") {
        std::vector<point> pts = {point(0,0), point(10,0), point(10,10), point(0,10)};
        rectangle r(pts, border_color, fill_color, false, nullptr);
        REQUIRE(r.get_control_points().size() == 4);
        REQUIRE_FALSE(r.is_filled());
        REQUIRE(r.get_control_points()[2].get_x() == 10.0);
        
        // Throws if size is not 4
        std::vector<point> bad_pts = {point(0,0), point(10,0)};
        REQUIRE_THROWS_AS(rectangle(bad_pts, border_color, fill_color, false, nullptr), std::logic_error);
    }

    SECTION("inside hit testing") {
        rectangle r(p1, p2, border_color, fill_color, true, nullptr);
        REQUIRE(r.inside(point(50.0, 25.0)));
        REQUIRE_FALSE(r.inside(point(200.0, 200.0)));
    }
}

TEST_CASE("triangle primitive coverage", "[figures][triangle]") {
    std::vector<point> tri_pts = {point(0.0, 0.0), point(100.0, 0.0), point(50.0, 86.6)};
    color border_color(0.0f, 0.0f, 1.0f);
    color fill_color(1.0f, 1.0f, 0.0f);

    SECTION("default constructor") {
        triangle t;
        REQUIRE(t.get_control_points().empty());
        REQUIRE(t.get_type_tag() == "triangle");
        REQUIRE(t.can_fill());
    }

    SECTION("parameterized constructor (vector points)") {
        triangle t(tri_pts, border_color, fill_color, true, nullptr);
        REQUIRE(t.get_control_points().size() == 3);
        REQUIRE(t.get_border_color() == border_color);
        REQUIRE(t.get_fill_color() == fill_color);
        REQUIRE(t.is_filled());
        
        REQUIRE(t.get_control_points()[0].get_x() == 0.0);
        REQUIRE(t.get_control_points()[1].get_x() == 100.0);
        REQUIRE(t.get_control_points()[2].get_y() == 86.6);

        // Throws if size != 3
        std::vector<point> bad_pts = {point(0,0), point(10,0)};
        REQUIRE_THROWS_AS(triangle(bad_pts, border_color, fill_color, true, nullptr), std::logic_error);
    }

    SECTION("inside hit testing") {
        triangle t(tri_pts, border_color, fill_color, true, nullptr);
        REQUIRE(t.inside(point(50.0, 30.0)));
        REQUIRE_FALSE(t.inside(point(200.0, 200.0)));
    }
}

TEST_CASE("ellipse primitive coverage", "[figures][ellipse]") {
    point p1(0.0, 0.0);
    point p2(100.0, 50.0);
    color border_color(1.0f, 1.0f, 1.0f);
    color fill_color(0.5f, 0.5f, 0.5f);

    SECTION("default constructor") {
        ellipse e;
        REQUIRE(e.get_control_points().empty());
        REQUIRE(e.get_type_tag() == "ellipse");
        REQUIRE(e.can_fill());
    }

    SECTION("parameterized constructor (p1, p2)") {
        ellipse e(p1, p2, border_color, fill_color, true, nullptr);
        // Ellipse constructor constructs center + x-radius-handle + y-radius-handle
        REQUIRE(e.get_control_points().size() == 3);
        
        // Bounding box of (0,0) to (100,50) has center at (50, 25)
        // Center control point (idx 0)
        REQUIRE(e.get_center().x == 50.0);
        REQUIRE(e.get_center().y == 25.0);
        
        // y-radius handle (idx 1): center.x, bb[0].y -> (50, 0)
        REQUIRE(e.get_control_points()[1].get_x() == 50.0);
        REQUIRE(e.get_control_points()[1].get_y() == 0.0);
        
        // x-radius handle (idx 2): bb[1].x, center.y -> (100, 25)
        REQUIRE(e.get_control_points()[2].get_x() == 100.0);
        REQUIRE(e.get_control_points()[2].get_y() == 25.0);

        // Verify bounding box spans the entire ellipse
        bounding_box box = e.get_bounding_box();
        auto box_pts = box.get_bounding_box();
        REQUIRE(box_pts.size() == 4);
        REQUIRE(box_pts[0].x == 0.0);
        REQUIRE(box_pts[0].y == 0.0);
        REQUIRE(box_pts[2].x == 100.0);
        REQUIRE(box_pts[2].y == 50.0);
    }

    SECTION("parameterized constructor (vector points)") {
        std::vector<point> pts = {point(50,25), point(50,0), point(100,25)};
        ellipse e(pts, border_color, fill_color, false, nullptr);
        REQUIRE(e.get_control_points().size() == 3);
        REQUIRE(e.get_center().x == 50.0);
        
        std::vector<point> bad_pts = {point(0,0), point(10,0)};
        REQUIRE_THROWS_AS(ellipse(bad_pts, border_color, fill_color, false, nullptr), std::logic_error);
    }

    SECTION("inside hit testing") {
        ellipse e(p1, p2, border_color, fill_color, true, nullptr);
        REQUIRE(e.inside(point(50.0, 25.0)));
        REQUIRE_FALSE(e.inside(point(200.0, 200.0)));
    }
}

TEST_CASE("bezier primitive coverage", "[figures][bezier]") {
    std::vector<point> pts = {point(0,0), point(50,100), point(100,0)};
    color border_color(0.5f, 0.5f, 0.5f);

    SECTION("default constructor") {
        bezier b;
        REQUIRE(b.get_control_points().empty());
        REQUIRE(b.get_type_tag() == "bezier");
        REQUIRE_FALSE(b.can_fill());
        REQUIRE_THROWS_AS(b.draw_fill(), std::logic_error);
    }

    SECTION("parameterized constructor") {
        bezier b(pts, border_color, nullptr);
        REQUIRE(b.get_control_points().size() == 3);
        REQUIRE(b.get_border_color() == border_color);
        REQUIRE(b.get_control_points()[1].get_y() == 100.0);
    }

    SECTION("draw_border throws when control points < 3") {
        std::vector<point> bad_pts = {point(0,0), point(10,10)};
        bezier b(bad_pts, border_color, nullptr);
        REQUIRE_THROWS_AS(b.draw_border(), std::logic_error);
    }

    SECTION("inside always returns false") {
        bezier b(pts, border_color, nullptr);
        REQUIRE_FALSE(b.inside(point(50.0, 50.0)));
    }
}


