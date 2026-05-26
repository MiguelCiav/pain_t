#include "app.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../figures/rectangle.h"
#include "../tools/ellipse_tool.h"
#include "../tools/line_tool.h"
#include "../tools/rect_tool.h"
#include "../tools/triangle_tool.h"
#include "../tools/bezier_tool.h"
#include "../tools/selection_tool.h"
#include <iostream>

app::app() : engine_2d(800, 600, "pain_t") {}

app::~app() {
  delete l_tool;
  delete r_tool;
  delete t_tool;
  delete e_tool;
  delete b_tool;
  delete s_tool;
}

void app::setup() {
  clear(background_color);
  std::cout << "pain_t engine initialized successfully." << std::endl;

  l_tool = new line_tool(this, this);
  r_tool = new rect_tool(this, this);
  t_tool = new triangle_tool(this, this);
  e_tool = new ellipse_tool(this, this);
  b_tool = new bezier_tool(this, this);
  s_tool = new selection_tool(this, this);
  active_tool = l_tool;
}

void app::on_key_down(int key) {
  if (ImGui::GetIO().WantCaptureKeyboard)
    return;
  if (active_tool)
    active_tool->on_key_down(key);
}

void app::on_mouse_button_down(int button, double x, double y) {
  if (ImGui::GetIO().WantCaptureMouse)
    return;
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

  for (figure *fig : main_scene.get_figures()) {
    fig->draw();
  }

  if (active_tool) {
    active_tool->draw_preview();
  }
}

void app::draw_ui() {
  ImGui::Begin("Tools");

  std::string current = active_tool ? active_tool->get_name() : "None";
  ImGui::Text("Active Tool: %s", current.c_str());
  ImGui::Separator();

  if (ImGui::Button("Selection Tool")) {
    active_tool = s_tool;
  }

  if (ImGui::Button("Line Tool")) {
    active_tool = l_tool;
  }

  if (ImGui::Button("Rectangle Tool")) {
    active_tool = r_tool;
  }

  if (ImGui::Button("Triangle Tool")) {
    active_tool = t_tool;
  }

  if (ImGui::Button("Ellipse Tool")) {
    active_tool = e_tool;
  }

  if (ImGui::Button("Bezier Tool")) {
    active_tool = b_tool;
  }

  ImGui::Separator();
  ImGui::ColorEdit3("Border Color", &border_color.r);
  ImGui::ColorEdit3("Fill Color", &fill_color.r);

  ImGui::End();
}
