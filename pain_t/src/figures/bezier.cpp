#include "bezier.h"
#include "../engine/engine_2d.h"
#include "figures/algebra.h"
#include "figures/control_point.h"
#include "figures/figure.h"
#include "rasterizer.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

bezier::bezier(std::vector<point> points, color border_color, engine_2d *engine)
    : figure(border_color, color(0, 0, 0), false, engine) {
  for (auto p : points) {
    control_points.push_back(control_point(p));
  }
}

bool bezier::can_fill() const { return false; }

void bezier::draw_fill() { throw std::logic_error("Cannot fill a bezier"); }

void bezier::draw_border() {
  if (control_points.size() < 3) {
    throw std::logic_error("bezier must have at least 3 control points");
  }
  std::vector<point> positions;
  positions.resize(control_points.size());
  std::transform(control_points.begin(), control_points.end(),
                 positions.begin(),
                 [](control_point cp) { return cp.get_position(); });
  rasterizer::bezier::draw(engine, positions, border_color);
}

std::vector<point> bezier::get_render_points() const {
  std::vector<point> result;
  std::vector<point> points;
  points.resize(control_points.size());
  std::transform(control_points.begin(), control_points.end(), points.begin(),
                 [](control_point p) { return p.get_position(); });

  int steps = static_cast<int>(1.0 / rasterizer::bezier::bezier_step + 0.5);
  for (int step = 0; step <= steps; ++step) {
    double t = static_cast<double>(step) / steps;
    std::vector<point> aux_points = points;
    int curr_size = points.size();
    for (int i = 0; i < points.size() - 1; i++) {
      for (int j = 0; j < curr_size - 1; j++) {
        aux_points[j] = ((1 - t) * aux_points[j]) + (t * aux_points[j + 1]);
      }
      curr_size--;
    }
    point current = point(static_cast<int>(aux_points[0].x),
                          static_cast<int>(aux_points[0].y));
    result.push_back(current);
  }

  return result;
}

bool bezier::on_border(point click) const {
  if (control_points.size() < 3) {
    throw std::logic_error(
        "cannot select a bezier with less than 3 control points");
  }
  std::vector<point> render_points = this->get_render_points();
  for (size_t i = 0; i < render_points.size() - 1; ++i) {
    if (algebra::line::distance_from_point(
            render_points[i], render_points[i + 1], click) < LINE_TOLERANCE) {
      return true;
    }
  }
  return false;
}

bool bezier::on_filling(point click) const { return false; }

std::string bezier::get_type_tag() const { return "bezier"; }

figure *bezier::clone() const {
  std::vector<point> pts;
  for (const auto &cp : control_points) {
    pts.push_back(cp.get_position());
  }
  bezier *cloned = new bezier(pts, border_color, engine);
  cloned->set_bordered(bordered);
  cloned->set_z_index(z_index);
  return cloned;
}

void bezier::increase_degree() {
  if (control_points.size() < 3) {
    throw std::logic_error(
        "cannot increase bezier degree with less than 3 control points");
  }
  std::vector<control_point> new_control_points;
  new_control_points.push_back(control_points.front());
  int n = control_points.size();
  for (int i = 1; i < n; i++) {
    double d_i = static_cast<double>(i);
    double d_n = static_cast<double>(n);
    point p_i = control_points[i].get_position();
    point p_im = control_points[i - 1].get_position();
    point result = (d_i / d_n) * p_im + (1.0 - (d_i / d_n)) * p_i;
    new_control_points.push_back(control_point(result));
  }
  new_control_points.push_back(control_points.back());
  control_points.clear();
  control_points = new_control_points;
}

point bezier::evaluate(double t) const {
  if (control_points.size() < 3) {
    throw std::logic_error("bezier must have at least 3 control points");
  }
  std::vector<point> pts;
  pts.resize(control_points.size());
  std::transform(control_points.begin(), control_points.end(), pts.begin(),
                 [](control_point cp) { return cp.get_position(); });

  int n = pts.size();
  std::vector<point> aux = pts;
  int curr_size = n;
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < curr_size - 1; j++) {
      aux[j] = ((1.0 - t) * aux[j]) + (t * aux[j + 1]);
    }
    curr_size--;
  }
  return aux[0];
}

std::pair<std::vector<point>, std::vector<point>> bezier::subdivide(double t) const {
  if (control_points.size() < 3) {
    throw std::logic_error("bezier must have at least 3 control points");
  }
  std::vector<point> pts;
  pts.resize(control_points.size());
  std::transform(control_points.begin(), control_points.end(), pts.begin(),
                 [](control_point cp) { return cp.get_position(); });

  int n = pts.size();
  std::vector<std::vector<point>> triangle(n, std::vector<point>(n));

  for (int i = 0; i < n; ++i) {
    triangle[0][i] = pts[i];
  }

  for (int r = 1; r < n; ++r) {
    for (int i = 0; i < n - r; ++i) {
      triangle[r][i] = (1.0 - t) * triangle[r - 1][i] + t * triangle[r - 1][i + 1];
    }
  }

  std::vector<point> left_pts;
  for (int r = 0; r < n; ++r) {
    left_pts.push_back(triangle[r][0]);
  }

  std::vector<point> right_pts;
  for (int r = n - 1; r >= 0; --r) {
    right_pts.push_back(triangle[r][n - 1 - r]);
  }

  return {left_pts, right_pts};
}
