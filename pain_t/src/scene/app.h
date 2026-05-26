#pragma once
#include "../engine/engine_2d.h"
#include "GLFW/glfw3.h"
#include "scene.h"
#include <vector>

// Forward declarations
class figure;
class i_tool;

class app : public engine_2d {
private:
  color background_color = color(0.85f, 0.85f, 0.85f);
  color border_color;
  color fill_color;
  scene main_scene;
  i_tool *active_tool = nullptr;
  i_tool *l_tool = nullptr;
  i_tool *r_tool = nullptr;
  i_tool *t_tool = nullptr;
  i_tool *e_tool = nullptr;
  i_tool *b_tool = nullptr;
  i_tool *s_tool = nullptr;

public:
  app();
  ~app();

  void setup() override;
  void on_key_down(int key) override;
  void on_mouse_button_down(int button, double x, double y) override;
  void on_mouse_button_up(int button, double x, double y) override;
  void on_mouse_move(double x, double y) override;
  void update(float deltaTime) override;
  void draw_ui() override;
  color get_border_color() { return border_color; }
  color get_fill_color() { return fill_color; }
  scene& get_scene() { return main_scene; }
  bool is_ctrl_pressed() const {
    return is_key_pressed(GLFW_KEY_LEFT_CONTROL) ||
           is_key_pressed(GLFW_KEY_RIGHT_CONTROL);
  }
  bool is_enter_pressed() const { return is_key_pressed(GLFW_KEY_ENTER); }
  bool is_escape_pressed() const { return is_key_pressed(GLFW_KEY_ESCAPE); }
};
