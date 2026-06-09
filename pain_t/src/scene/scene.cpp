#include "scene.h"
#include "../engine/engine_2d.h"
#include "../figures/rasterizer.h"
#include "commands/command_history.h"
#include "commands/i_command.h"
#include "quad_tree.h"
#include <algorithm>
#include <stdexcept>
#include <unordered_set>

scene::~scene() {
  for (figure *fig : figures) {
    delete fig;
  }
  delete tree;
}

void scene::init_tree(double x, double y, double width, double height) {
  if (width <= 0 || height <= 0) {
    throw std::invalid_argument(
        "Scene width and height must be greater than zero");
  }
  if (tree) {
    delete tree;
  }
  tree = new quad_tree(bounding_box(point(x, y), point(x + width, y),
                                    point(x + width, y + height), point(x, y + height)),
                       0);
  for (figure *fig : figures) {
    tree->insert(fig);
  }
}

void scene::add_figure(figure *f) {
  if (!f) {
    throw std::logic_error("Cannot add a nullptr figure to scene");
  }
  figures.push_back(f);
  if (tree) {
    tree->insert(f);
  }
}

void scene::remove_figure(figure *f) {
  if (!f) {
    throw std::logic_error("Cannot remove a nullptr figure from scene");
  }
  auto it = std::find(figures.begin(), figures.end(), f);
  if (it == figures.end()) {
    throw std::logic_error(
        "Cannot remove a figure that is not present in the scene");
  }
  figures.erase(it);
  if (tree) {
    tree->clear();
    for (figure *fig : figures) {
      tree->insert(fig);
    }
  }
}

void scene::notify_figure_moved(figure *f) {
  if (!f) {
    throw std::logic_error("Cannot notify from a nullptr figure on scene");
  }
  if (tree) {
    tree->clear();
    for (figure *fig : figures) {
      tree->insert(fig);
    }
  }
}

void scene::clear() {
  deselect();
  for (figure *fig : figures) {
    delete fig;
  }
  figures.clear();
  if (tree) {
    tree->clear();
  }
  history.clear();
}

void scene::reorder_figures(int source_idx, int target_idx) {
  if (source_idx < 0 || source_idx >= static_cast<int>(figures.size())) {
    throw std::out_of_range("Source index is out of bounds");
  }
  if (target_idx < 0 || target_idx >= static_cast<int>(figures.size())) {
    throw std::out_of_range("Target index is out of bounds");
  }
  if (source_idx == target_idx) {
    return;
  }
  figure *temp = figures[source_idx];
  figures.erase(figures.begin() + source_idx);
  figures.insert(figures.begin() + target_idx, temp);
}

const std::vector<figure *> &scene::get_figures() const { return figures; }

std::vector<figure *> &scene::get_figures() { return figures; }

void scene::select(figure *f) {
  if (!f) {
    throw std::logic_error("Cannot select a nullptr figure");
  }
  deselect();
  selected_figure = f;
  if (selected_figure) {
    selected_figure->select();
  }
}

void scene::deselect() {
  if (selected_figure) {
    selected_figure->unselect();
  }
  selected_figure = nullptr;
}

figure *scene::get_selected_figure() const { return selected_figure; }

figure *scene::query(point click) const {
  if (!tree) {
    throw std::logic_error("Can't make scene queries without a tree");
  }

  query_anim_nodes.clear();
  query_anim_index = -1;
  query_anim_timer = 0.0f;

  bounding_box click_box(
      point(click.x - CLICK_TOLERANCE, click.y - CLICK_TOLERANCE),
      point(click.x + CLICK_TOLERANCE, click.y - CLICK_TOLERANCE),
      point(click.x + CLICK_TOLERANCE, click.y + CLICK_TOLERANCE),
      point(click.x - CLICK_TOLERANCE, click.y + CLICK_TOLERANCE));

  std::unordered_set<figure *> candidates = tree->query(click_box, nullptr, &query_anim_nodes);

  if (!query_anim_nodes.empty()) {
    query_anim_index = 0;
  }

  if (candidates.empty()) {
    return nullptr;
  }

  for (auto it = figures.rbegin(); it != figures.rend(); ++it) {
    if (candidates.count(*it) > 0) {
      return *it;
    }
  }

  return nullptr;
}

void scene::draw_all(engine_2d *engine) const {
  for (figure *fig : figures) {
    fig->draw();
  }
}

void scene::draw_quad_tree(engine_2d *engine) const {
  if (tree) {
    tree->draw(engine);
  }

  if (query_anim_index >= 0) {
    color red_color(1.0f, 0.0f, 0.0f);
    for (int i = 0; i <= query_anim_index && i < static_cast<int>(query_anim_nodes.size()); ++i) {
      std::vector<point> pts = query_anim_nodes[i].get_bounding_box();
      if (pts.size() >= 4) {
        rasterizer::line::draw(engine, pts[0], pts[1], red_color);
        rasterizer::line::draw(engine, pts[1], pts[2], red_color);
        rasterizer::line::draw(engine, pts[2], pts[3], red_color);
        rasterizer::line::draw(engine, pts[3], pts[0], red_color);
      }
    }
  }
}

void scene::update_animation(float deltaTime) const {
  if (query_anim_index >= 0 && query_anim_index < static_cast<int>(query_anim_nodes.size())) {
    query_anim_timer += deltaTime;
    if (query_anim_timer >= 0.15f) {
      query_anim_timer = 0.0f;
      query_anim_index++;
    }
  }
}

void scene::undo() {
  if (history.can_undo())
    history.undo();
}

void scene::redo() {
  if (history.can_redo())
    history.redo();
}

void scene::execute(i_command *cmd) {
  if (!cmd) {
    throw std::logic_error("Can't execute a null command");
  }
  cmd->execute();
  history.add(cmd);
}
