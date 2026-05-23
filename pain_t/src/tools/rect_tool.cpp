#include "rect_tool.h"
#include "../engine/engine_2d.h"
#include "../figures/figure.h"
#include "../figures/rectangle.h"
#include "../scene/app.h"
#include <string>

rect_tool::rect_tool(engine_2d *engine, std::vector<figure *> &figures)
    : engine(engine), scene_figures(figures) {}

void rect_tool::on_mouse_down(int button, point p) {
  is_drawing = true;
  starting_point = p;
  ending_point = p;
}

void rect_tool::on_mouse_move(point p) {
  if (!is_drawing) {
    return;
  }
  ending_point = p;
}

void rect_tool::on_mouse_up(int button, point p) {
  if (!is_drawing) {
    return;
  }
  is_drawing = false;
  ending_point = p;

  std::vector<point> rect_points = {point(starting_point.x, starting_point.y),
                                    point(ending_point.x, starting_point.y),
                                    point(ending_point.x, ending_point.y),
                                    point(starting_point.x, ending_point.y)};

  app *application = static_cast<app *>(engine);
  color border_color = application->get_border_color();
  color fill_color = application->get_fill_color();
  figure *new_rect =
      new rectangle(rect_points, border_color, fill_color, true, engine);
  scene_figures.push_back(new_rect);
}

void rect_tool::on_key_down(int key) {}

void rect_tool::draw_preview() {
  if (!is_drawing) {
    return;
  }
  std::vector<point> rect_points = {point(starting_point.x, starting_point.y),
                                    point(ending_point.x, starting_point.y),
                                    point(ending_point.x, ending_point.y),
                                    point(starting_point.x, ending_point.y)};
  app *application = static_cast<app *>(engine);
  color border_color = application->get_border_color();
  color fill_color = application->get_fill_color();
  rectangle temp_rect(rect_points, border_color, fill_color, true, engine);
  temp_rect.draw();
}

std::string rect_tool::get_name() { return "rect_tool"; }
