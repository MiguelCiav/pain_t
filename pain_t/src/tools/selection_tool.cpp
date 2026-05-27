#include "selection_tool.h"
#include "../engine/engine_2d.h"
#include "../figures/bounding_box.h"
#include "../figures/figure.h"
#include "../figures/rasterizer.h"
#include "../scene/app.h"
#include "../scene/scene.h"
#include <iostream>
#include <string>

selection_tool::selection_tool(engine_2d *engine, app *application)
    : i_tool(engine, application) {}

void selection_tool::on_mouse_down(int button, point p) {
  figure *clicked = application->get_scene().query(p);

  if (clicked) {
    application->get_scene().select(clicked);
  } else {
    application->get_scene().deselect();
  }
}

void selection_tool::on_mouse_move(point p) {}

void selection_tool::on_mouse_up(int button, point p) {}

void selection_tool::on_key_down(int key) {}

void selection_tool::draw_preview() {
  figure *selected = application->get_scene().get_selected_figure();
  if (selected) {
    bounding_box bb = selected->get_bounding_box();
    std::vector<point> pts = bb.get_bounding_box();

    // Draw bounding box border in dashed or solid gray
    color selection_color(0.5f, 0.5f, 0.5f);
    rasterizer::line::draw(engine, pts[0], pts[1], selection_color);
    rasterizer::line::draw(engine, pts[1], pts[2], selection_color);
    rasterizer::line::draw(engine, pts[2], pts[3], selection_color);
    rasterizer::line::draw(engine, pts[3], pts[0], selection_color);
  }
}

std::string selection_tool::get_name() { return "selection_tool"; }
