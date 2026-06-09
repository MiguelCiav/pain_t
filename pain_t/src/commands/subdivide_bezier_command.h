#pragma once
#include "figures/bezier.h"
#include "i_command.h"
#include "scene/scene.h"
#include <vector>

class subdivide_bezier_command : public i_command {
private:
  scene *_scene;
  bezier *_original;
  bezier *_left;
  bezier *_right;

public:
  subdivide_bezier_command(scene *s, bezier *orig, bezier *left, bezier *right)
      : _scene(s), _original(orig), _left(left), _right(right) {}

  ~subdivide_bezier_command() {
    bool left_in_scene = false;
    bool right_in_scene = false;
    bool original_in_scene = false;

    for (figure *f : _scene->get_figures()) {
      if (f == _left) left_in_scene = true;
      if (f == _right) right_in_scene = true;
      if (f == _original) original_in_scene = true;
    }

    if (!left_in_scene) delete _left;
    if (!right_in_scene) delete _right;
    if (!original_in_scene) delete _original;
  }

  void execute() override {
    _scene->remove_figure(_original);
    _scene->add_figure(_left);
    _scene->add_figure(_right);
    _scene->deselect();
  }

  void undo() override {
    _scene->remove_figure(_left);
    _scene->remove_figure(_right);
    _scene->add_figure(_original);
    _scene->select(_original);
  }
};
