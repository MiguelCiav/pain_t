#include "app.h"
#include "../commands/delete_figure_command.h"
#include "../commands/move_figure_command.h"
#include "../commands/create_figure_command.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../figures/rectangle.h"
#include "../tools/bezier_tool.h"
#include "../tools/ellipse_tool.h"
#include "../tools/line_tool.h"
#include "../tools/rect_tool.h"
#include "../tools/selection_tool.h"
#include "../tools/triangle_tool.h"
#include "scene/scene.h"
#include "scene_serializer.h"

app::app(int width, int height) : engine_2d(width, height, "pain_t") {}

app::~app() {
  for (i_tool *tool : tools) {
    delete tool;
  }
  delete clipboard;
}

// APP SETUP
void app::setup() {
  clear(main_scene.get_background_color());
  main_scene.init_tree(sidebar_width, 0.0, get_width() - sidebar_width, get_height());
  register_tools();
  register_shortcuts();
}

void app::update(float deltaTime) {
  clear(main_scene.get_background_color());
  main_scene.draw_all(this);
  if (show_quad_tree) {
    main_scene.draw_quad_tree(this);
    main_scene.update_animation(deltaTime);
  }
  if (active_tool) {
    active_tool->draw_preview();
  }
  if (status_timer > 0.0f) {
    status_timer -= deltaTime;
    if (status_timer <= 0.0f) {
      status_message = "";
    }
  }
}

void app::register_tools() {
  tools.push_back(new selection_tool(this, this));
  tools.push_back(new line_tool(this, this));
  tools.push_back(new rect_tool(this, this));
  tools.push_back(new triangle_tool(this, this));
  tools.push_back(new ellipse_tool(this, this));
  tools.push_back(new bezier_tool(this, this));
  active_tool = tools[1]; // Line Tool is the default active tool
}

void app::register_shortcuts() {
  s_manager.register_shortcut(GLFW_KEY_Z, true,
                              [this]() { main_scene.undo(); });
  s_manager.register_shortcut(GLFW_KEY_Y, true,
                              [this]() { main_scene.redo(); });
  s_manager.register_shortcut(GLFW_KEY_Q, false,
                              [this]() { show_quad_tree = !show_quad_tree; });
  s_manager.register_shortcut(GLFW_KEY_DELETE, false, [this]() {
    figure *selected = main_scene.get_selected_figure();
    if (selected) {
      main_scene.execute(new delete_figure_command(&main_scene, selected));
    }
  });
  s_manager.register_shortcut(GLFW_KEY_BACKSPACE, false, [this]() {
    figure *selected = main_scene.get_selected_figure();
    if (selected) {
      main_scene.execute(new delete_figure_command(&main_scene, selected));
    }
  });
  s_manager.register_shortcut(GLFW_KEY_S, true, [this]() { save_scene(); });
  s_manager.register_shortcut(GLFW_KEY_O, true, [this]() { load_scene(); });

  // WASD translation shortcuts
  s_manager.register_shortcut(GLFW_KEY_W, false, [this]() {
    figure *selected = main_scene.get_selected_figure();
    if (selected) {
      main_scene.execute(new move_figure_command(selected, &main_scene, point(0.0, -5.0)));
    }
  });
  s_manager.register_shortcut(GLFW_KEY_S, false, [this]() {
    figure *selected = main_scene.get_selected_figure();
    if (selected) {
      main_scene.execute(new move_figure_command(selected, &main_scene, point(0.0, 5.0)));
    }
  });
  s_manager.register_shortcut(GLFW_KEY_A, false, [this]() {
    figure *selected = main_scene.get_selected_figure();
    if (selected) {
      main_scene.execute(new move_figure_command(selected, &main_scene, point(-5.0, 0.0)));
    }
  });
  s_manager.register_shortcut(GLFW_KEY_D, false, [this]() {
    figure *selected = main_scene.get_selected_figure();
    if (selected) {
      main_scene.execute(new move_figure_command(selected, &main_scene, point(5.0, 0.0)));
    }
  });

  // Clipboard shortcuts
  s_manager.register_shortcut(GLFW_KEY_C, true, [this]() {
    figure *selected = main_scene.get_selected_figure();
    if (selected) {
      delete clipboard;
      clipboard = selected->clone();
    }
  });
  s_manager.register_shortcut(GLFW_KEY_X, true, [this]() {
    figure *selected = main_scene.get_selected_figure();
    if (selected) {
      delete clipboard;
      clipboard = selected->clone();
      main_scene.execute(new delete_figure_command(&main_scene, selected));
    }
  });
  s_manager.register_shortcut(GLFW_KEY_V, true, [this]() {
    if (clipboard) {
      figure *pasted = clipboard->clone();
      pasted->move(point(20.0, 20.0));
      delete clipboard;
      clipboard = pasted->clone();

      main_scene.execute(new create_figure_command(&main_scene, pasted));
      main_scene.select(pasted);
    }
  });
}

// UI
void app::draw_ui() {
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(sidebar_width, get_height()), ImGuiCond_Always);
  ImGui::Begin("Tools", nullptr,
               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoCollapse);

  gui.render(this);

  ImGui::End();
}

// EVENTS
void app::on_key_down(int key) {
  if (ImGui::GetIO().WantCaptureKeyboard)
    return;

  if (s_manager.handle_key(key, is_ctrl_pressed()))
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

// SCENE STORAGE
void app::save_scene() {
  if (scene_serializer::save(main_scene, save_load_path)) {
    set_status("Successfully saved to " + std::string(save_load_path),
               color(0.1f, 0.8f, 0.1f));
  } else {
    set_status("Failed to save to " + std::string(save_load_path),
               color(0.9f, 0.1f, 0.1f));
  }
}

void app::load_scene() {
  if (scene_serializer::load_into(save_load_path, main_scene, this)) {
    set_status("Successfully loaded from " + std::string(save_load_path),
               color(0.1f, 0.8f, 0.1f));
  } else {
    set_status("Failed to load from " + std::string(save_load_path),
               color(0.9f, 0.1f, 0.1f));
  }
}

// STATUS
void app::set_status(const std::string &msg, const color &col) {
  status_message = msg;
  status_color = col;
  status_timer = 5.0f;
}
