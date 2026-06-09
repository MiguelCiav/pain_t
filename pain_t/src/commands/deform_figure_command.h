#pragma once
#include "figures/figure.h"
#include "i_command.h"
#include "scene/scene.h"

class deform_figure_command : public i_command {
private:
  figure *_figure;
  scene *_scene;
  std::vector<point> _old_points;
  std::vector<point> _new_points;

public:
  deform_figure_command(figure *f, scene *s, const std::vector<point>& old_pts, const std::vector<point>& new_pts)
      : _figure(f), _scene(s), _old_points(old_pts), _new_points(new_pts) {}

  void execute() override {
    for (size_t i = 0; i < _new_points.size(); ++i) {
      _figure->set_control_point(i, _new_points[i]);
    }
    _scene->notify_figure_moved(_figure);
  }

  void undo() override {
    for (size_t i = 0; i < _old_points.size(); ++i) {
      _figure->set_control_point(i, _old_points[i]);
    }
    _scene->notify_figure_moved(_figure);
  }
};
