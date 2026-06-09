#pragma once
#include "../figures/figure.h"
#include "../figures/point.h"
#include "commands/command_history.h"
#include "commands/i_command.h"
#include <vector>

class quad_tree;

const double CLICK_TOLERANCE = 3;

class scene {
private:
  command_history history;
  color background_color = color(0.85f, 0.85f, 0.85f);
  color active_border_color = color(0.0f, 0.0f, 0.0f);
  color active_fill_color = color(1.0f, 1.0f, 1.0f);
  figure *selected_figure = nullptr;
  std::vector<figure *> figures;
  bool active_bordered = true;
  bool active_filled = true;
  quad_tree *tree = nullptr;

  // Animation state for QuadTree selection
  mutable std::vector<bounding_box> query_anim_nodes;
  mutable int query_anim_index = -1;
  mutable float query_anim_timer = 0.0f;

public:
  scene() = default;
  ~scene();

  void init_tree(double x, double y, double width, double height);
  void add_figure(figure *f);
  void remove_figure(figure *f);
  void notify_figure_moved(figure *f);
  void clear();
  void reorder_figures(int source_idx, int target_idx);
  const std::vector<figure *> &get_figures() const;
  std::vector<figure *> &get_figures();

  void select(figure *f);
  void deselect();
  figure *get_selected_figure() const;
  figure *query(point click) const;

  // Drawing
  void draw_all(engine_2d *engine) const;
  void draw_quad_tree(engine_2d *engine) const;
  void update_animation(float deltaTime) const;

  // Document Color State
  color get_background_color() const { return background_color; }
  void set_background_color(color c) { background_color = c; }
  color get_active_border_color() const { return active_border_color; }
  void set_active_border_color(color c) { active_border_color = c; }
  color get_active_fill_color() const { return active_fill_color; }
  void set_active_fill_color(color c) { active_fill_color = c; }
  bool is_active_bordered() const { return active_bordered; }
  void set_active_bordered(bool b) { active_bordered = b; }
  bool is_active_filled() const { return active_filled; }
  void set_active_filled(bool f) { active_filled = f; }

  // Commands
  void undo();
  void redo();
  void execute(i_command *cmd);
};
