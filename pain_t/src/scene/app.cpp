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

  main_scene.init_tree(get_width(), get_height());

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

  if (show_quad_tree) {
    main_scene.draw_quad_tree(this);
  }

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

  ImGui::Checkbox("Show QuadTree (Q)", &show_quad_tree);

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

  ImGui::Separator();
  ImGui::Text("Layers (Z-Index)");
  ImGui::TextDisabled("Drag & Drop to reorder");

  ImGui::BeginChild("LayersList", ImVec2(0, 150), true);
  std::vector<figure *> &figures = main_scene.get_figures();
  int size = static_cast<int>(figures.size());

  for (int n = 0; n < size; n++) {
    int fig_idx = size - 1 - n;
    figure *fig = figures[fig_idx];

    std::string item_label = std::to_string(n + 1) + ". " +
                             fig->get_type_tag() + "##" +
                             std::to_string(reinterpret_cast<uintptr_t>(fig));
    bool is_selected = (fig == main_scene.get_selected_figure());

    if (ImGui::Selectable(item_label.c_str(), is_selected)) {
      main_scene.select(fig);
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
      ImGui::SetDragDropPayload("DND_FIGURE_INDEX", &n, sizeof(int));
      ImGui::Text("Move %s", fig->get_type_tag().c_str());
      ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
      if (const ImGuiPayload *payload =
              ImGui::AcceptDragDropPayload("DND_FIGURE_INDEX")) {
        IM_ASSERT(payload->DataSize == sizeof(int));
        int source_n = *(const int *)payload->Data;
        int target_n = n;

        int source_idx = size - 1 - source_n;
        int target_idx = size - 1 - target_n;

        main_scene.reorder_figures(source_idx, target_idx);
      }
      ImGui::EndDragDropTarget();
    }
  }
  ImGui::EndChild();

  ImGui::End();
}
