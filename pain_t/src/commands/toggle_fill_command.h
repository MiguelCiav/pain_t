#pragma once
#include "figures/figure.h"
#include "i_command.h"

class toggle_fill_command : public i_command {
private:
  figure *_figure;
  bool _old_val;
  bool _new_val;

public:
  toggle_fill_command(figure *f, bool new_val)
      : _figure(f), _new_val(new_val) {
    _old_val = f->is_filled();
  }

  void execute() override {
    _figure->set_filled(_new_val);
  }

  void undo() override {
    _figure->set_filled(_old_val);
  }
};
