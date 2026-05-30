#include "scene.h"
#include "../engine/engine_2d.h"
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

void scene::init_tree(double width, double height) {
  if (width <= 0 || height <= 0) {
    throw std::invalid_argument(
        "Scene width and height must be greater than zero");
  }
  if (tree) {
    delete tree;
  }
  tree = new quad_tree(bounding_box(point(0, 0), point(width, 0),
                                    point(width, height), point(0, height)),
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

  bounding_box click_box(
      point(click.x - CLICK_TOLERANCE, click.y - CLICK_TOLERANCE),
      point(click.x + CLICK_TOLERANCE, click.y - CLICK_TOLERANCE),
      point(click.x + CLICK_TOLERANCE, click.y + CLICK_TOLERANCE),
      point(click.x - CLICK_TOLERANCE, click.y + CLICK_TOLERANCE));

  std::unordered_set<figure *> candidates = tree->query(click_box);

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
}
