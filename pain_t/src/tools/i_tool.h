#pragma once
#include "../engine/engine_2d.h"
#include "../figures/figure.h"
#include "../figures/point.h"
#include "../scene/app.h"
#include <string>

class i_tool {
protected:
  engine_2d *engine;
  std::vector<figure *> &scene_figures;
  app *application;

public:
  virtual ~i_tool() = default;
  i_tool(engine_2d *engine, std::vector<figure *> &scene_figures,
         app *application);

  virtual void on_mouse_down(int button, point p) = 0;
  virtual void on_mouse_move(point p) = 0;
  virtual void on_mouse_up(int button, point p) = 0;
  virtual void on_key_down(int key) = 0;
  virtual void draw_preview() = 0;
  virtual std::string get_name() = 0;
};