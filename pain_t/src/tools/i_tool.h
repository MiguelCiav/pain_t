#pragma once
#include "../figures/point.h"
#include <string>

class engine_2d;
class figure;
class app;

class i_tool {
protected:
  engine_2d *engine;
  app *application;

public:
  virtual ~i_tool() = default;
  i_tool(engine_2d *engine, app *application);

  virtual void on_mouse_down(int button, point p) = 0;
  virtual void on_mouse_move(point p) = 0;
  virtual void on_mouse_up(int button, point p) = 0;
  virtual void on_key_down(int key) = 0;
  virtual void draw_preview() = 0;
  virtual void draw_settings() {}
  virtual std::string get_name() = 0;
  virtual std::string get_label() = 0;
  virtual std::string get_icon() = 0;
};