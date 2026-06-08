#pragma once
#include "figures/figure.h"
#include "i_command.h"
#include "scene/scene.h"
#include <vector>

class clear_scene_command : public i_command {
private:
  scene *_scene;
  std::vector<figure *> _figures;
  figure *_selected_figure = nullptr;
  bool _owns_figures = false;

public:
  clear_scene_command(scene *s) : _scene(s) {
    _figures = s->get_figures();
    _selected_figure = s->get_selected_figure();
  }

  ~clear_scene_command() {
    if (_owns_figures) {
      for (figure *fig : _figures) {
        delete fig;
      }
    }
  }

  void execute() override {
    _scene->deselect();
    // Remove figures back-to-front
    for (auto it = _figures.rbegin(); it != _figures.rend(); ++it) {
      _scene->remove_figure(*it);
    }
    _owns_figures = true;
  }

  void undo() override {
    for (figure *fig : _figures) {
      _scene->add_figure(fig);
    }
    if (_selected_figure) {
      _scene->select(_selected_figure);
    }
    _owns_figures = false;
  }
};
