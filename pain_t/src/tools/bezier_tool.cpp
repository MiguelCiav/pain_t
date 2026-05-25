#include "bezier_tool.h"
#include "../engine/engine_2d.h"
#include "../figures/bezier.h"
#include "../figures/figure.h"
#include "../scene/app.h"
#include "figures/line.h"
#include "figures/point.h"
#include <iostream>
#include <string>

bezier_tool::bezier_tool(engine_2d *engine, std::vector<figure *> &figures,
                         app *application)
    : i_tool(engine, figures, application) {}

void bezier_tool::on_mouse_down(int button, point p) {
  is_drawing = true;
  current_mouse_pos = p;
}

void bezier_tool::on_mouse_move(point p) {
  if (is_drawing) {
    current_mouse_pos = p;
  }
}

void bezier_tool::on_mouse_up(int button, point p) {
  if (!is_drawing) {
    return;
  }
  points.push_back(p);
  draw_preview();
}

void bezier_tool::on_key_down(int key) {
  if (!is_drawing) {
    return;
  }
  if (points.size() < 3) {
    is_drawing = false;
    points.clear();
    return;
  }
  if (application->is_enter_pressed() || application->is_escape_pressed()) {
    is_drawing = false;
    figure *new_bezier =
        new bezier(points, application->get_border_color(), engine);
    scene_figures.push_back(new_bezier);
    points.clear();
  }
}

void bezier_tool::draw_preview() {
  if (!is_drawing) {
    return;
  }

  bool pushed_preview = false;
  if (points.empty() || !(points.back() == current_mouse_pos)) {
    points.push_back(current_mouse_pos);
    pushed_preview = true;
  }

  if (points.size() >= 2) {
    for (int i = 0; i < points.size() - 1; i++) {
      line preview_line(points[i], points[i + 1], color(0.6, 0.6, 0.6), engine);
      preview_line.draw_border();
    }
  }

  if (points.size() >= 3) {
    bezier temp_bezier(points, application->get_border_color(), engine);
    temp_bezier.draw_border();
  }

  if (pushed_preview) {
    points.pop_back();
  }
}

std::string bezier_tool::get_name() { return "bezier_tool"; }
