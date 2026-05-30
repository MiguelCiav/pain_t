#include "selection_tool.h"
#include "../engine/engine_2d.h"
#include "../figures/algebra.h"
#include "../figures/bounding_box.h"
#include "../figures/figure.h"
#include "../figures/rasterizer.h"
#include "../scene/app.h"
#include "../scene/scene.h"
#include <string>

selection_tool::selection_tool(engine_2d *engine, app *application)
    : i_tool(engine, application) {}

bool selection_tool::try_select_control_point(figure *selected, point p) {
  std::vector<control_point> &control_pts = selected->get_control_points();
  for (size_t i = 0; i < control_pts.size(); ++i) {
    if (selected->get_type_tag() == "ellipse" && i == 0) {
      continue;
    }
    point cp_pos = control_pts[i].get_position();
    if (algebra::distance(p, cp_pos) <= 6.0) {
      is_deforming = true;
      active_control_point_idx = static_cast<int>(i);
      last_mouse_point = p;
      return true;
    }
  }
  return false;
}

bool selection_tool::try_select_center(figure *selected, point p) {
  point center = selected->get_bounding_box().get_center();
  if (algebra::distance(p, center) <= 6.0) {
    is_dragging = true;
    last_mouse_point = p;
    return true;
  }
  return false;
}

void selection_tool::query_new_figure(point p) {
  figure *clicked = application->get_scene().query(p);
  if (clicked) {
    application->get_scene().select(clicked);
  } else {
    application->get_scene().deselect();
  }
}

void selection_tool::on_mouse_down(int button, point p) {
  if (button == 0) {
    figure *selected = application->get_scene().get_selected_figure();
    if (selected) {
      if (try_select_control_point(selected, p))
        return;
      if (try_select_center(selected, p))
        return;
    }
  }
  query_new_figure(p);
}

void selection_tool::deform_figure(figure *selected, point p) {
  if (!selected || active_control_point_idx < 0 ||
      active_control_point_idx >=
          static_cast<int>(selected->get_control_points().size())) {
    return;
  }

  point shift = p - last_mouse_point;
  control_point &cp = selected->get_control_points()[active_control_point_idx];

  if (selected->get_type_tag() == "ellipse") {
    point center = selected->get_control_points()[0].get_position();
    if (active_control_point_idx == 1) {
      double new_y = cp.get_y() + shift.y;
      cp.set_position(center.x, new_y);
    } else if (active_control_point_idx == 2) {
      double new_x = cp.get_x() + shift.x;
      cp.set_position(new_x, center.y);
    }
  } else {
    cp.set_position(cp.get_x() + shift.x, cp.get_y() + shift.y);
  }

  application->get_scene().notify_figure_moved(selected);
  last_mouse_point = p;
}

void selection_tool::drag_figure(figure *selected, point p) {
  if (selected) {
    point shift = p - last_mouse_point;
    selected->move(shift);
    application->get_scene().notify_figure_moved(selected);
    last_mouse_point = p;
  }
}

void selection_tool::on_mouse_move(point p) {
  figure *selected = application->get_scene().get_selected_figure();
  if (is_deforming) {
    deform_figure(selected, p);
  } else if (is_dragging) {
    drag_figure(selected, p);
  }
}

void selection_tool::on_mouse_up(int button, point p) {
  if (button == 0) {
    is_dragging = false;
    is_deforming = false;
    active_control_point_idx = -1;
  }
}

void selection_tool::on_key_down(int key) {}

void selection_tool::draw_bounding_box(figure *selected) {
  bounding_box bb = selected->get_bounding_box();
  std::vector<point> pts = bb.get_bounding_box();
  color selection_color(0.5f, 0.5f, 0.5f);
  rasterizer::line::draw(engine, pts[0], pts[1], selection_color);
  rasterizer::line::draw(engine, pts[1], pts[2], selection_color);
  rasterizer::line::draw(engine, pts[2], pts[3], selection_color);
  rasterizer::line::draw(engine, pts[3], pts[0], selection_color);
}

void selection_tool::draw_selection_cross(figure *selected) {
  point center = selected->get_bounding_box().get_center();
  color cross_color(0.12f, 0.53f, 0.9f);
  int half_size = 5;
  point p_left(center.x - half_size, center.y);
  point p_right(center.x + half_size, center.y);
  point p_top(center.x, center.y - half_size);
  point p_bottom(center.x, center.y + half_size);
  rasterizer::line::draw(engine, p_left, p_right, cross_color);
  rasterizer::line::draw(engine, p_top, p_bottom, cross_color);
}

void selection_tool::draw_bezier_guidelines(figure *selected) {
  if (selected->get_type_tag() == "bezier") {
    const auto &control_pts = selected->get_control_points();
    color guide_color(0.5f, 0.5f, 0.5f);
    for (size_t i = 0; i + 1 < control_pts.size(); ++i) {
      rasterizer::line::draw(engine, control_pts[i].get_position(),
                             control_pts[i + 1].get_position(), guide_color);
    }
  }
}

void selection_tool::draw_control_points(figure *selected) {
  color white(1.0f, 1.0f, 1.0f);
  color black(0.0f, 0.0f, 0.0f);
  int sq_half_size = 3;
  const auto &control_pts = selected->get_control_points();
  for (size_t i = 0; i < control_pts.size(); ++i) {
    if (selected->get_type_tag() == "ellipse" && i == 0) {
      continue;
    }
    point cp_pos = control_pts[i].get_position();
    int cp_x = static_cast<int>(std::round(cp_pos.x));
    int cp_y = static_cast<int>(std::round(cp_pos.y));

    for (int y = cp_y - sq_half_size; y <= cp_y + sq_half_size; y++) {
      rasterizer::line::draw_horizontal(engine, cp_x - sq_half_size,
                                        cp_x + sq_half_size, y, white);
    }
    rasterizer::line::draw_horizontal(engine, cp_x - sq_half_size,
                                      cp_x + sq_half_size, cp_y - sq_half_size,
                                      black);
    rasterizer::line::draw_horizontal(engine, cp_x - sq_half_size,
                                      cp_x + sq_half_size, cp_y + sq_half_size,
                                      black);
    rasterizer::line::draw_vertical(engine, cp_y - sq_half_size,
                                    cp_y + sq_half_size, cp_x - sq_half_size,
                                    black);
    rasterizer::line::draw_vertical(engine, cp_y - sq_half_size,
                                    cp_y + sq_half_size, cp_x + sq_half_size,
                                    black);
  }
}

void selection_tool::draw_preview() {
  figure *selected = application->get_scene().get_selected_figure();
  if (selected) {
    draw_bounding_box(selected);
    draw_selection_cross(selected);
    draw_bezier_guidelines(selected);
    draw_control_points(selected);
  }
}

std::string selection_tool::get_name() { return "selection_tool"; }
