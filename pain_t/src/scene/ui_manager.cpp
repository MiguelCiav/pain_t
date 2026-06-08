#include "ui_manager.h"
#include "app.h"
#include "scene.h"
#include "../figures/figure.h"
#include "../tools/i_tool.h"
#include "../commands/change_color_command.h"
#include "../commands/reorder_figures_command.h"
#include "../commands/clear_scene_command.h"
#include "../commands/toggle_border_command.h"
#include "../commands/toggle_fill_command.h"
#include <imgui.h>
#include <vector>
#include <string>

void ui_manager::render(app *application) {
  draw_tool_selector(application);
  draw_canvas_actions(application);
  draw_file_operations(application);
  draw_color_settings(application);
  draw_layers_panel(application);
}

void ui_manager::draw_tool_selector(app *application) {
  i_tool *active = application->get_active_tool();
  std::string current = active ? active->get_name() : "None";

  ImGui::Text("Active Tool: %s", current.c_str());
  ImGui::Separator();

  float avail_width = ImGui::GetContentRegionAvail().x;
  float spacing = ImGui::GetStyle().ItemSpacing.x;
  float btn_size = (avail_width - spacing) / 2.0f;

  const std::vector<i_tool *> &tools = application->get_tools();
  for (size_t i = 0; i < tools.size(); ++i) {
    i_tool *tool = tools[i];
    if (i % 2 != 0) {
      ImGui::SameLine();
    }

    bool is_active = (tool == active);
    if (is_active) {
      ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
    }

    if (ImGui::Button((tool->get_icon() + "##" + tool->get_name()).c_str(), ImVec2(btn_size, btn_size))) {
      application->set_active_tool(tool);
    }

    if (is_active) {
      ImGui::PopStyleColor(2);
    }

    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("%s", tool->get_label().c_str());
    }
  }

  ImGui::Separator();

  active = application->get_active_tool();
  if (active) {
    active->draw_settings();
  }
}

void ui_manager::draw_canvas_actions(app *application) {
  ImGui::Separator();
  scene &sc = application->get_scene();

  if (ImGui::Button("Clear Scene")) {
    if (!sc.get_figures().empty()) {
      sc.execute(new clear_scene_command(&sc));
    }
  }

  if (ImGui::Button("undo")) {
    sc.undo();
  }

  if (ImGui::Button("redo")) {
    sc.redo();
  }

  bool show = application->is_showing_quad_tree();
  if (ImGui::Checkbox("Show QuadTree (Q)", &show)) {
    application->set_show_quad_tree(show);
  }
}

void ui_manager::draw_file_operations(app *application) {
  ImGui::Separator();
  ImGui::Text("File Operations");
  ImGui::InputText("##FilePath", application->get_save_load_path(), 256);

  if (ImGui::Button("Save Canvas")) {
    application->save_scene();
  }
  ImGui::SameLine();
  if (ImGui::Button("Load Canvas")) {
    application->load_scene();
  }

  const std::string &status_message = application->get_status_message();
  if (!status_message.empty()) {
    color status_color = application->get_status_color();
    ImGui::TextColored(
        ImVec4(status_color.r, status_color.g, status_color.b, 1.0f), "%s",
        status_message.c_str());
  }
}

void ui_manager::draw_color_settings(app *application) {
  ImGui::Separator();
  scene &sc = application->get_scene();

  ImGui::Text("Border Color");
  color border = sc.get_active_border_color();

  static color border_start_color;
  static bool border_color_active = false;

  if (ImGui::ColorEdit3("##Border Color", &border.r)) {
    if (!border_color_active) {
      if (sc.get_selected_figure()) {
        border_start_color = sc.get_selected_figure()->get_border_color();
      } else {
        border_start_color = sc.get_active_border_color();
      }
      border_color_active = true;
    }
    sc.set_active_border_color(border);
    if (sc.get_selected_figure())
      sc.get_selected_figure()->set_border_color(border);
  }

  if (ImGui::IsItemDeactivatedAfterEdit()) {
    border_color_active = false;
    if (sc.get_selected_figure()) {
      figure *selected = sc.get_selected_figure();
      selected->set_border_color(border_start_color);
      sc.execute(new change_color_command(selected, color_type::border, border_start_color, border));
    }
  }

  bool bordered = sc.get_selected_figure() ? sc.get_selected_figure()->is_bordered() : sc.is_active_bordered();
  if (ImGui::Checkbox("Show Border", &bordered)) {
    sc.set_active_bordered(bordered);
    if (sc.get_selected_figure()) {
      sc.execute(new toggle_border_command(sc.get_selected_figure(), bordered));
    }
  }

  ImGui::Text("Fill Color");
  color fill = sc.get_active_fill_color();

  static color fill_start_color;
  static bool fill_color_active = false;

  if (ImGui::ColorEdit3("##Fill Color", &fill.r)) {
    if (!fill_color_active) {
      if (sc.get_selected_figure()) {
        fill_start_color = sc.get_selected_figure()->get_fill_color();
      } else {
        fill_start_color = sc.get_active_fill_color();
      }
      fill_color_active = true;
    }
    sc.set_active_fill_color(fill);
    if (sc.get_selected_figure())
      sc.get_selected_figure()->set_fill_color(fill);
  }

  if (ImGui::IsItemDeactivatedAfterEdit()) {
    fill_color_active = false;
    if (sc.get_selected_figure()) {
      figure *selected = sc.get_selected_figure();
      selected->set_fill_color(fill_start_color);
      sc.execute(new change_color_command(selected, color_type::fill, fill_start_color, fill));
    }
  }

  bool can_fill = sc.get_selected_figure() ? sc.get_selected_figure()->can_fill() : true;
  if (can_fill) {
    bool filled = sc.get_selected_figure() ? sc.get_selected_figure()->is_filled() : sc.is_active_filled();
    if (ImGui::Checkbox("Show Fill", &filled)) {
      sc.set_active_filled(filled);
      if (sc.get_selected_figure() && sc.get_selected_figure()->can_fill()) {
        sc.execute(new toggle_fill_command(sc.get_selected_figure(), filled));
      }
    }
  }

  ImGui::Text("Background Color");
  color bg = sc.get_background_color();
  if (ImGui::ColorEdit3("##Background Color", &bg.r)) {
    sc.set_background_color(bg);
  }
}

void ui_manager::draw_layers_panel(app *application) {
  ImGui::Separator();
  ImGui::Text("Layers (Z-Index)");
  ImGui::TextDisabled("Drag & Drop to reorder");

  ImGui::BeginChild("LayersList", ImVec2(0, 150), true);
  scene &sc = application->get_scene();
  std::vector<figure *> &figures = sc.get_figures();
  int size = static_cast<int>(figures.size());

  for (int n = 0; n < size; n++) {
    int fig_idx = size - 1 - n;
    figure *fig = figures[fig_idx];

    std::string item_label =
        std::to_string(n + 1) + ". " + fig->get_type_tag() + "##" +
        std::to_string(reinterpret_cast<unsigned long>(fig));
    bool is_selected = (fig == sc.get_selected_figure());

    if (ImGui::Selectable(item_label.c_str(), is_selected)) {
      sc.select(fig);
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
        sc.execute(new reorder_figures_command(&sc, source_idx, target_idx));
      }
      ImGui::EndDragDropTarget();
    }
  }
  ImGui::EndChild();
}
