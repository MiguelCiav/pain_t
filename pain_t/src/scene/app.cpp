#include "app.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../figures/rectangle.h"
#include "../tools/line_tool.h"
#include <iostream>

app::app() : engine_2d(800, 600, "pain_t") {}

app::~app() {
  delete active_tool;
  for (figure *fig : figures) {
    delete fig;
  }
}

void app::setup() {
  clear(background_color);
  std::cout << "pain_t engine initialized successfully." << std::endl;
  
  // Add an example rectangle
  std::vector<point> rect_points = {
    point(200, 200), // Top-left
    point(600, 200), // Top-right
    point(600, 400), // Bottom-right
    point(200, 400)  // Bottom-left
  };
  figure* sample_rect = new rectangle(rect_points, color(0.8f, 0.1f, 0.1f), color(0.2f, 0.5f, 0.9f), true, this);
  figures.push_back(sample_rect);

  active_tool = new line_tool(this, figures);
}

void app::on_key_down(int key) {
  if (active_tool)
    active_tool->on_key_down(key);
}

void app::on_mouse_button_down(int button, double x, double y) {
  if (active_tool)
    active_tool->on_mouse_down(button, point(x, y));
}

void app::on_mouse_button_up(int button, double x, double y) {
  if (active_tool)
    active_tool->on_mouse_up(button, point(x, y));
}

void app::on_mouse_move(double x, double y) {
  if (active_tool)
    active_tool->on_mouse_move(point(x, y));
}

void app::update(float deltaTime) {
  clear(background_color);

  for (figure *fig : figures) {
    fig->draw();
  }

  if (active_tool) {
    active_tool->draw_preview();
  }
}

void app::draw_ui() {}

int main() {
  app engine;
  engine.run();
  return 0;
}
