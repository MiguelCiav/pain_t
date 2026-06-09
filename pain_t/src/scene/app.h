#pragma once
#include "../engine/engine_2d.h"
#include "GLFW/glfw3.h"
#include "scene.h"
#include "shortcut_manager.h"
#include "ui_manager.h"

// Forward declarations
class figure;
class i_tool;

class app : public engine_2d {
private:
  scene main_scene;
  shortcut_manager s_manager;
  i_tool *active_tool = nullptr;
  std::vector<i_tool *> tools;
  bool show_quad_tree = false;
  char save_load_path[256] = "drawing.p_t";
  std::string status_message = "";
  color status_color = color(0.0f, 0.0f, 0.0f);
  float status_timer = 0.0f;
  double sidebar_width = 200.0;
  ui_manager gui;
  figure *clipboard = nullptr;

  void set_status(const std::string& msg, const color& col);

  void register_shortcuts();
  void register_tools();

public:
  app(int width = 800, int height = 600);
  ~app();

  void setup() override;
  void on_key_down(int key) override;
  void on_mouse_button_down(int button, double x, double y) override;
  void on_mouse_button_up(int button, double x, double y) override;
  void on_mouse_move(double x, double y) override;
  void update(float deltaTime) override;
  void draw_ui() override;

  void save_scene();
  void load_scene();

  scene &get_scene() { return main_scene; }
  i_tool *get_active_tool() const { return active_tool; }
  void set_active_tool(i_tool *tool);
  const std::vector<i_tool *> &get_tools() const { return tools; }
  
  bool is_showing_quad_tree() const { return show_quad_tree; }
  void set_show_quad_tree(bool show) { show_quad_tree = show; }

  char *get_save_load_path() { return save_load_path; }
  const std::string &get_status_message() const { return status_message; }
  color get_status_color() const { return status_color; }

  double get_sidebar_width() const { return sidebar_width; }
  void set_sidebar_width(double width) { sidebar_width = width; }

  bool is_ctrl_pressed() const {
    return is_key_pressed(GLFW_KEY_LEFT_CONTROL) ||
           is_key_pressed(GLFW_KEY_RIGHT_CONTROL);
  }
  bool is_enter_pressed() const { return is_key_pressed(GLFW_KEY_ENTER); }
  bool is_escape_pressed() const { return is_key_pressed(GLFW_KEY_ESCAPE); }
};
