#pragma once
#include "i_tool.h"
#include <stdexcept>

i_tool::i_tool(engine_2d *engine, std::vector<figure *> &scene_figures,
               app *application)
    : scene_figures(scene_figures) {
  if (engine == nullptr) {
    throw std::logic_error("Cannot create a tool without an engine");
  }
  if (application == nullptr) {
    throw std::logic_error("Cannot create a tool without an application");
  }
  this->engine = engine;
  this->application = application;
}