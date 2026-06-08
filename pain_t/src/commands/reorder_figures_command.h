#pragma once
#include "i_command.h"
#include "scene/scene.h"

class reorder_figures_command : public i_command {
private:
  scene *_scene;
  int _source_idx;
  int _target_idx;

public:
  reorder_figures_command(scene *s, int source_idx, int target_idx)
      : _scene(s), _source_idx(source_idx), _target_idx(target_idx) {}

  void execute() override {
    _scene->reorder_figures(_source_idx, _target_idx);
  }

  void undo() override {
    _scene->reorder_figures(_target_idx, _source_idx);
  }
};
