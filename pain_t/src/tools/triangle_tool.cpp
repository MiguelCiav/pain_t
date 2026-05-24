#include "triangle_tool.h"
#include "../engine/engine_2d.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../figures/triangle.h"
#include "../scene/app.h"
#include <string>

triangle_tool::triangle_tool(engine_2d *engine, std::vector<figure *> &figures)
    : engine(engine), scene_figures(figures) {}

void triangle_tool::on_mouse_down(int button, point p) {
  if (state == 0) {
    p1 = p;
    p2 = p;
    state = 1;
  } else if (state == 1) {
    p2 = p;
    p3 = p;
    state = 2;
  } else if (state == 2) {
    p3 = p;
    state = 0;

    std::vector<point> tri_points = {p1, p2, p3};
    color border_color = color(0, 0, 0);
    color fill_color = color(0, 0, 0);
    if (engine != nullptr) {
      app *application = static_cast<app *>(engine);
      border_color = application->get_border_color();
      fill_color = application->get_fill_color();
    }
    figure *new_tri =
        new triangle(tri_points, border_color, fill_color, true, engine);
    scene_figures.push_back(new_tri);
  }
}

void triangle_tool::on_mouse_move(point p) {
  if (state == 1) {
    p2 = p;
  } else if (state == 2) {
    p3 = p;
  }
}

void triangle_tool::on_mouse_up(int button, point p) {
  // We handle clicks in on_mouse_down for this multi-click tool
}

void triangle_tool::on_key_down(int key) {}

void triangle_tool::draw_preview() {
  if (state == 1) {
    color border_color = color(0, 0, 0);
    if (engine != nullptr) {
      app *application = static_cast<app *>(engine);
      border_color = application->get_border_color();
    }
    line temp_line(p1, p2, border_color, engine);
    temp_line.draw_border();
  } else if (state == 2) {
    color border_color = color(0, 0, 0);
    color fill_color = color(0, 0, 0);
    if (engine != nullptr) {
      app *application = static_cast<app *>(engine);
      border_color = application->get_border_color();
      fill_color = application->get_fill_color();
    }
    std::vector<point> tri_points = {p1, p2, p3};
    triangle temp_tri(tri_points, border_color, fill_color, true, engine);
    temp_tri.draw();
  }
}

std::string triangle_tool::get_name() { return "triangle_tool"; }
