#pragma once
#include "figures/figure.h"
#include "i_command.h"
#include "scene/scene.h"

class scale_figure_command : public i_command {
private:
  figure *_figure;
  scene *_scene;
  double _factor;

public:
  scale_figure_command(figure *f, scene *s, double factor)
      : _figure(f), _scene(s), _factor(factor) {}

  void execute() override {
    _figure->scale(_factor);
    _scene->notify_figure_moved(_figure);
  }

  void undo() override {
    _figure->scale(1.0 / _factor);
    _scene->notify_figure_moved(_figure);
  }
};
