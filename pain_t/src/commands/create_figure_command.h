#pragma once
#include "figures/figure.h"
#include "i_command.h"
#include "scene/scene.h"

class create_figure_command : public i_command {
private:
  scene *_scene;
  figure *_figure;

  bool _owns_figure = true;

public:
  create_figure_command(scene *_scene, figure *_figure)
      : _scene(_scene), _figure(_figure), _owns_figure(true) {}

  ~create_figure_command() {
    if (_owns_figure) {
      delete _figure;
    }
  }

  void execute() override {
    _scene->add_figure(_figure);
    _owns_figure = false;
  }

  void undo() override {
    _scene->remove_figure(_figure);
    _owns_figure = true;
  }
};