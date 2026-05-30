#include "line_tool.h"
#include "../engine/engine_2d.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../scene/app.h"
#include "commands/create_figure_command.h"
#include "commands/i_command.h"
#include <string>

line_tool::line_tool(engine_2d *engine, app *application)
    : i_tool(engine, application) {}

void line_tool::on_mouse_down(int button, point p) {
  is_drawing = true;
  starting_point = p;
  ending_point = p;
}

void line_tool::on_mouse_move(point p) {
  if (!is_drawing) {
    return;
  }
  ending_point = p;
}

void line_tool::on_mouse_up(int button, point p) {
  if (!is_drawing) {
    return;
  }
  is_drawing = false;
  ending_point = p;
  if (starting_point == ending_point) {
    return;
  }
  figure *new_line =
      new line(starting_point, ending_point,
               application->get_scene().get_active_border_color(), engine);

  i_command *cmd =
      new create_figure_command(&application->get_scene(), new_line);

  application->get_scene().execute(cmd);
}

void line_tool::on_key_down(int key) {}

void line_tool::draw_preview() {
  if (!is_drawing) {
    return;
  }
  line temp_line(starting_point, ending_point,
                 application->get_scene().get_active_border_color(), engine);
  temp_line.draw_border();
}

std::string line_tool::get_name() { return "line_tool"; }
