#pragma once
#include "../figures/bezier.h"
#include "../figures/control_point.h"
#include "i_command.h"
#include "scene/scene.h"
#include <vector>

class increase_degree_command : public i_command {
private:
  bezier *_bezier;
  scene *_scene;
  std::vector<control_point> _old_control_points;
  std::vector<control_point> _new_control_points;

public:
  increase_degree_command(bezier *b, scene *s)
      : _bezier(b), _scene(s) {
    _old_control_points = b->get_control_points();
  }

  void execute() override {
    if (_new_control_points.empty()) {
      _bezier->increase_degree();
      _new_control_points = _bezier->get_control_points();
    } else {
      _bezier->get_control_points() = _new_control_points;
    }
    _scene->notify_figure_moved(_bezier);
  }

  void undo() override {
    _bezier->get_control_points() = _old_control_points;
    _scene->notify_figure_moved(_bezier);
  }
};
