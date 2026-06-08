#include "triangle_tool.h"
#include "../engine/engine_2d.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../figures/triangle.h"
#include "../scene/app.h"
#include "commands/create_figure_command.h"
#include "commands/i_command.h"
#include <string>

triangle_tool::triangle_tool(engine_2d *engine, app *application)
    : i_tool(engine, application) {}

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
    figure *new_tri = new triangle(
        tri_points, application->get_scene().get_active_border_color(),
        application->get_scene().get_active_fill_color(),
        application->get_scene().is_active_filled(), engine);
    new_tri->set_bordered(application->get_scene().is_active_bordered());

    i_command *cmd =
        new create_figure_command(&application->get_scene(), new_tri);

    application->get_scene().execute(cmd);
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
    line temp_line(p1, p2, application->get_scene().get_active_border_color(),
                   engine);
    temp_line.draw_border();
  } else if (state == 2) {
    std::vector<point> tri_points = {p1, p2, p3};
    triangle temp_tri(
        tri_points, application->get_scene().get_active_border_color(),
        application->get_scene().get_active_fill_color(),
        application->get_scene().is_active_filled(), engine);
    temp_tri.set_bordered(application->get_scene().is_active_bordered());
    temp_tri.draw();
  }
}

std::string triangle_tool::get_name() { return "triangle_tool"; }
