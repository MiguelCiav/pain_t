#pragma once
#include "figures/figure.h"
#include "i_command.h"

enum class color_type {
  border,
  fill
};

class change_color_command : public i_command {
private:
  figure *_figure;
  color_type _type;
  color _old_color;
  color _new_color;

public:
  change_color_command(figure *f, color_type type, color old_color, color new_color)
      : _figure(f), _type(type), _old_color(old_color), _new_color(new_color) {}

  void execute() override {
    if (_type == color_type::border) {
      _figure->set_border_color(_new_color);
    } else {
      _figure->set_fill_color(_new_color);
    }
  }

  void undo() override {
    if (_type == color_type::border) {
      _figure->set_border_color(_old_color);
    } else {
      _figure->set_fill_color(_old_color);
    }
  }
};
