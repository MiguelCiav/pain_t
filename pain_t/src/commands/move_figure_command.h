#pragma once
#include "figures/figure.h"
#include "i_command.h"
#include "scene/scene.h"

class move_figure_command : public i_command {
private:
  figure *_figure;
  scene *_scene;
  point _delta;

public:
  move_figure_command(figure *f, scene *s, point delta)
      : _figure(f), _scene(s), _delta(delta) {}

  void execute() override {
    _figure->move(_delta);
    _scene->notify_figure_moved(_figure);
  }

  void undo() override {
    _figure->move(-_delta);
    _scene->notify_figure_moved(_figure);
  }
};
