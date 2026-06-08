#pragma once
#include "figures/figure.h"
#include "i_command.h"

class toggle_border_command : public i_command {
private:
  figure *_figure;
  bool _old_val;
  bool _new_val;

public:
  toggle_border_command(figure *f, bool new_val)
      : _figure(f), _new_val(new_val) {
    _old_val = f->is_bordered();
  }

  void execute() override {
    _figure->set_bordered(_new_val);
  }

  void undo() override {
    _figure->set_bordered(_old_val);
  }
};
