#pragma once
#include "figures/figure.h"
#include "i_command.h"
#include "scene/scene.h"

class delete_figure_command : public i_command {
private:
  scene *_scene;
  figure *_figure;

public:
  delete_figure_command(scene *_scene, figure *_figure)
      : _scene(_scene), _figure(_figure) {}
  void execute() override { 
    if (_scene->get_selected_figure() == _figure) {
        _scene->deselect();
    }
    _scene->remove_figure(_figure); 
  }
  void undo() override { _scene->add_figure(_figure); }
};
