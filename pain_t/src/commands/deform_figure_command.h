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
    _figure->set_control_point(_cp_idx, _new_pos);
    _scene->notify_figure_moved(_figure);
  }

  void undo() override {
    _figure->set_control_point(_cp_idx, _old_pos);
    _scene->notify_figure_moved(_figure);
  }
};
