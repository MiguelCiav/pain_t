#pragma once
#include "../engine/engine_2d.h"
#include <vector>

// Forward declarations
class figure;
class i_tool;

class app : public engine_2d {
private:
  color background_color = color(0.85f, 0.85f, 0.85f);
  color border_color;
  color fill_color;
  std::vector<figure *> figures;
  i_tool *active_tool = nullptr;
  i_tool *l_tool = nullptr;
  i_tool *r_tool = nullptr;

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
};
