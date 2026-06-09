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

  bool _is_executed = false;

public:
  subdivide_bezier_command(scene *s, bezier *orig, bezier *left, bezier *right)
      : _scene(s), _original(orig), _left(left), _right(right), _is_executed(false) {}

  ~subdivide_bezier_command() {
    if (_is_executed) {
      delete _original;
    } else {
      delete _left;
      delete _right;
    }
  }

  void execute() override {
    _scene->remove_figure(_original);
    _scene->add_figure(_left);
    _scene->add_figure(_right);
    _scene->deselect();
    _is_executed = true;
  }

  void undo() override {
    _scene->remove_figure(_left);
    _scene->remove_figure(_right);
    _scene->add_figure(_original);
    _scene->select(_original);
    _is_executed = false;
  }
};
