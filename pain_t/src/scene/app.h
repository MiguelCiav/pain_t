#pragma once
#include "../engine/engine_2d.h"
#include "GLFW/glfw3.h"
#include "scene.h"
#include "shortcut_manager.h"

// Forward declarations
class figure;
class i_tool;

class app : public engine_2d {
private:
  scene main_scene;
  shortcut_manager s_manager;
  i_tool *active_tool = nullptr;
  i_tool *l_tool = nullptr;
  i_tool *r_tool = nullptr;
  i_tool *t_tool = nullptr;
  i_tool *e_tool = nullptr;
  i_tool *b_tool = nullptr;
  i_tool *s_tool = nullptr;
  bool show_quad_tree = false;
  char save_load_path[256] = "drawing.p_t";
  std::string status_message = "";
  color status_color = color(0.0f, 0.0f, 0.0f);
  float status_timer = 0.0f;

  void save_scene();
  void load_scene();
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
  scene &get_scene() { return main_scene; }
  bool is_ctrl_pressed() const {
    return is_key_pressed(GLFW_KEY_LEFT_CONTROL) ||
           is_key_pressed(GLFW_KEY_RIGHT_CONTROL);
  }
  bool is_enter_pressed() const { return is_key_pressed(GLFW_KEY_ENTER); }
  bool is_escape_pressed() const { return is_key_pressed(GLFW_KEY_ESCAPE); }
};
