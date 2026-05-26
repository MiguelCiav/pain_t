#include "app.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../figures/rectangle.h"
#include "../tools/bezier_tool.h"
#include "../tools/ellipse_tool.h"
#include "../tools/line_tool.h"
#include "../tools/rect_tool.h"
#include "../tools/selection_tool.h"
#include "../tools/triangle_tool.h"
#include <iostream>

app::app(int width, int height) : engine_2d(width, height, "pain_t") {}

app::~app() {
  delete l_tool;
  delete r_tool;
  delete t_tool;
  delete e_tool;
  delete b_tool;
  delete s_tool;
}

void app::setup() {
  clear(main_scene.get_background_color());
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
  clear(main_scene.get_background_color());
  main_scene.draw_all(this);

  if (active_tool) {
    active_tool->draw_preview();
  }
}

void app::draw_ui() {
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(200, get_height()), ImGuiCond_Always);
  ImGui::Begin("Tools", nullptr,
               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoCollapse);

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

  if (ImGui::Button("Clear Scene")) {
    main_scene.clear();
  }

  ImGui::Separator();

  ImGui::Text("Border Color");
  color border = main_scene.get_active_border_color();
  if (ImGui::ColorEdit3("##Border Color", &border.r)) {
    main_scene.set_active_border_color(border);
  }

  ImGui::Text("Fill Color");
  color fill = main_scene.get_active_fill_color();
  if (ImGui::ColorEdit3("##Fill Color", &fill.r)) {
    main_scene.set_active_fill_color(fill);
  }

  ImGui::Text("Background Color");
  color bg = main_scene.get_background_color();
  if (ImGui::ColorEdit3("##Background Color", &bg.r)) {
    main_scene.set_background_color(bg);
  }

  ImGui::End();
}
