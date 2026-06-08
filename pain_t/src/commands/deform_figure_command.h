#pragma once
#include "figures/figure.h"
#include "i_command.h"
#include "scene/scene.h"

class deform_figure_command : public i_command {
private:
  figure *_figure;
  scene *_scene;
  int _cp_idx;
  point _old_pos;
  point _new_pos;

public:
  deform_figure_command(figure *f, scene *s, int cp_idx, point old_pos, point new_pos)
      : _figure(f), _scene(s), _cp_idx(cp_idx), _old_pos(old_pos), _new_pos(new_pos) {}

  void execute() override {
    if (_figure->get_type_tag() == "ellipse") {
      auto &control_pts = _figure->get_control_points();
      point center = control_pts[0].get_position();
      if (_cp_idx == 1) {
        control_pts[_cp_idx].set_position(center.x, _new_pos.y);
      } else if (_cp_idx == 2) {
        control_pts[_cp_idx].set_position(_new_pos.x, center.y);
      }
    } else {
      _figure->get_control_points()[_cp_idx].set_position(_new_pos);
    }
    _scene->notify_figure_moved(_figure);
  }

  void undo() override {
    if (_figure->get_type_tag() == "ellipse") {
      auto &control_pts = _figure->get_control_points();
      point center = control_pts[0].get_position();
      if (_cp_idx == 1) {
        control_pts[_cp_idx].set_position(center.x, _old_pos.y);
      } else if (_cp_idx == 2) {
        control_pts[_cp_idx].set_position(_old_pos.x, center.y);
      }
    } else {
      _figure->get_control_points()[_cp_idx].set_position(_old_pos);
    }
    _scene->notify_figure_moved(_figure);
  }
};
