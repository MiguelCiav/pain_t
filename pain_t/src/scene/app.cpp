#include "app.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../figures/rectangle.h"
#include "../tools/line_tool.h"
#include "../tools/rect_tool.h"
#include "../tools/triangle_tool.h"
#include <iostream>

app::app() : engine_2d(800, 600, "pain_t") {}

app::~app() {
  delete l_tool;
  delete r_tool;
  delete t_tool;
  for (figure *fig : figures) {
    delete fig;
  }
}

void app::setup() {
  clear(background_color);
  std::cout << "pain_t engine initialized successfully." << std::endl;
  
  l_tool = new line_tool(this, figures);
  r_tool = new rect_tool(this, figures);
  t_tool = new triangle_tool(this, figures);
  active_tool = l_tool;
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

void app::draw_ui() {
  ImGui::Begin("Tools");
  
  std::string current = active_tool ? active_tool->get_name() : "None";
  ImGui::Text("Active Tool: %s", current.c_str());
  ImGui::Separator();
  
  if (ImGui::Button("Line Tool")) {
      active_tool = l_tool;
  }
  
  if (ImGui::Button("Rectangle Tool")) {
      active_tool = r_tool;
  }
  
  if (ImGui::Button("Triangle Tool")) {
      active_tool = t_tool;
  }
  
  ImGui::Separator();
  ImGui::ColorEdit3("Border Color", &border_color.r);
  ImGui::ColorEdit3("Fill Color", &fill_color.r);

  ImGui::End();
}

int main() {
  app engine;
  engine.run();
  return 0;
}
