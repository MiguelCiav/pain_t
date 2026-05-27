#include "selection_tool.h"
#include "../engine/engine_2d.h"
#include "../figures/bounding_box.h"
#include "../figures/figure.h"
#include "../figures/rasterizer.h"
#include "../scene/app.h"
#include "../scene/scene.h"
#include <string>

selection_tool::selection_tool(engine_2d *engine, app *application)
    : i_tool(engine, application) {}

void selection_tool::on_mouse_down(int button, point p) {
  if (button == 0) {
    figure *selected = application->get_scene().get_selected_figure();
    if (selected) {
      // First check if clicked on any of the control points of the selected
      // figure
      std::vector<control_point> &control_pts = selected->get_control_points();
      for (size_t i = 0; i < control_pts.size(); ++i) {
        // Special case: ellipse center point (index 0) shouldn't be
        // interactable
        if (selected->get_type_tag() == "ellipse" && i == 0) {
          continue;
        }
        point cp_pos = control_pts[i].get_position();
        double dx = p.x - cp_pos.x;
        double dy = p.y - cp_pos.y;
        double dist_sq = dx * dx + dy * dy;
        double tolerance = 6.0;
        if (dist_sq <= tolerance * tolerance) {
          is_deforming = true;
          active_control_point_idx = static_cast<int>(i);
          last_mouse_point = p;
          return;
        }
      }

      // Second check if clicked on the center cross
      point center = selected->get_bounding_box().get_center();
      double dx = p.x - center.x;
      double dy = p.y - center.y;
      double dist_sq = dx * dx + dy * dy;
      double tolerance = 6.0;
      if (dist_sq <= tolerance * tolerance) {
        is_dragging = true;
        last_mouse_point = p;
        return;
      }
    }
  }

  figure *clicked = application->get_scene().query(p);

  if (clicked) {
    application->get_scene().select(clicked);
  } else {
    application->get_scene().deselect();
  }
}

void selection_tool::on_mouse_move(point p) {
  if (is_deforming) {
    figure *selected = application->get_scene().get_selected_figure();
    if (selected && active_control_point_idx >= 0 &&
        active_control_point_idx <
            static_cast<int>(selected->get_control_points().size())) {
      point shift = p - last_mouse_point;
      control_point &cp =
          selected->get_control_points()[active_control_point_idx];

      if (selected->get_type_tag() == "ellipse") {
        point center = selected->get_control_points()[0].get_position();
        if (active_control_point_idx == 1) {
          // Height/y-radius handle: move ONLY vertically relative to the
          // center!
          double new_y = cp.get_y() + shift.y;
          cp.set_position(center.x, new_y);
        } else if (active_control_point_idx == 2) {
          // Width/x-radius handle: move ONLY horizontally relative to the
          // center!
          double new_x = cp.get_x() + shift.x;
          cp.set_position(new_x, center.y);
        }
      } else {
        // Standard deformation for other figures
        cp.set_position(cp.get_x() + shift.x, cp.get_y() + shift.y);
      }

      application->get_scene().notify_figure_moved(selected);
      last_mouse_point = p;
    }
  } else if (is_dragging) {
    figure *selected = application->get_scene().get_selected_figure();
    if (selected) {
      point shift = p - last_mouse_point;
      selected->move(shift);
      application->get_scene().notify_figure_moved(selected);
      last_mouse_point = p;
    }
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

void selection_tool::draw_preview() {
  figure *selected = application->get_scene().get_selected_figure();
  if (selected) {
    bounding_box bb = selected->get_bounding_box();
    std::vector<point> pts = bb.get_bounding_box();

    // Draw bounding box border in dashed or solid gray
    color selection_color(0.5f, 0.5f, 0.5f);
    rasterizer::line::draw(engine, pts[0], pts[1], selection_color);
    rasterizer::line::draw(engine, pts[1], pts[2], selection_color);
    rasterizer::line::draw(engine, pts[2], pts[3], selection_color);
    rasterizer::line::draw(engine, pts[3], pts[0], selection_color);

    // Draw selection cross at the center in premium vibrant blue
    point center = selected->get_bounding_box().get_center();
    color cross_color(0.12f, 0.53f, 0.9f);
    int half_size = 5;
    point p_left(center.x - half_size, center.y);
    point p_right(center.x + half_size, center.y);
    point p_top(center.x, center.y - half_size);
    point p_bottom(center.x, center.y + half_size);

    rasterizer::line::draw(engine, p_left, p_right, cross_color);
    rasterizer::line::draw(engine, p_top, p_bottom, cross_color);

    // Draw Bezier control point connection lines
    if (selected->get_type_tag() == "bezier") {
      const auto &control_pts = selected->get_control_points();
      color guide_color(0.5f, 0.5f, 0.5f); // gray guide line
      for (size_t i = 0; i + 1 < control_pts.size(); ++i) {
        rasterizer::line::draw(engine, control_pts[i].get_position(),
                               control_pts[i + 1].get_position(), guide_color);
      }
    }

    // Draw little white squares with black border on the control points
    color white(1.0f, 1.0f, 1.0f);
    color black(0.0f, 0.0f, 0.0f);
    int sq_half_size = 3; // 7x7 pixel square
    const auto &control_pts = selected->get_control_points();
    for (size_t i = 0; i < control_pts.size(); ++i) {
      // Special case: ellipse center point (index 0) shouldn't be drawn
      if (selected->get_type_tag() == "ellipse" && i == 0) {
        continue;
      }
      point cp_pos = control_pts[i].get_position();
      int cp_x = static_cast<int>(std::round(cp_pos.x));
      int cp_y = static_cast<int>(std::round(cp_pos.y));

      // Fill with white
      for (int y = cp_y - sq_half_size; y <= cp_y + sq_half_size; y++) {
        rasterizer::line::draw_horizontal(engine, cp_x - sq_half_size,
                                          cp_x + sq_half_size, y, white);
      }
      // Outline with black
      rasterizer::line::draw_horizontal(engine, cp_x - sq_half_size,
                                        cp_x + sq_half_size,
                                        cp_y - sq_half_size, black);
      rasterizer::line::draw_horizontal(engine, cp_x - sq_half_size,
                                        cp_x + sq_half_size,
                                        cp_y + sq_half_size, black);
      rasterizer::line::draw_vertical(engine, cp_y - sq_half_size,
                                      cp_y + sq_half_size, cp_x - sq_half_size,
                                      black);
      rasterizer::line::draw_vertical(engine, cp_y - sq_half_size,
                                      cp_y + sq_half_size, cp_x + sq_half_size,
                                      black);
    }
  }
}

std::string selection_tool::get_name() { return "selection_tool"; }
