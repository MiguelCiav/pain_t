#include "quad_tree.h"
#include "../engine/engine_2d.h"
#include "../figures/rasterizer.h"
#include "figures/bounding_box.h"
#include "figures/figure.h"
#include <algorithm>

void quad_tree::create_children() {
  std::vector<point> bounds = this->bounds.get_bounding_box();
  point center = this->bounds.get_center();
  this->children[0] =
      new quad_tree(bounding_box({bounds[0], center}), this->level + 1);
  this->children[1] =
      new quad_tree(bounding_box({center, bounds[1]}), this->level + 1);
  this->children[2] =
      new quad_tree(bounding_box({center, bounds[2]}), this->level + 1);
  this->children[3] =
      new quad_tree(bounding_box({bounds[3], center}), this->level + 1);
}

bool quad_tree::intersects(bounding_box box) const {
  std::vector<point> this_pts = this->bounds.get_bounding_box();
  std::vector<point> other_pts = box.get_bounding_box();
  if (this_pts.size() < 4 || other_pts.size() < 4) {
    return false;
  }
  return (this_pts[0].x <= other_pts[2].x && this_pts[2].x >= other_pts[0].x &&
          this_pts[0].y <= other_pts[2].y && this_pts[2].y >= other_pts[0].y);
}

void quad_tree::push_elements() {
  for (size_t i = 0; i < elements.size(); i++) {
    if (children[0]->intersects(elements[i]->get_bounding_box()))
      children[0]->insert(elements[i]);
    if (children[1]->intersects(elements[i]->get_bounding_box()))
      children[1]->insert(elements[i]);
    if (children[2]->intersects(elements[i]->get_bounding_box()))
      children[2]->insert(elements[i]);
    if (children[3]->intersects(elements[i]->get_bounding_box()))
      children[3]->insert(elements[i]);
  }
  this->elements.clear();
}

void quad_tree::insert(figure *fig) {
  if (this->level == LEVELS_LIMIT) {
    this->elements.push_back(fig);
    return;
  }
  if (this->is_leaf() && this->elements.size() < ELEMENTS_LIMIT) {
    this->elements.push_back(fig);
    if(this->elements.size() >= ELEMENTS_LIMIT) {
      this->create_children();
      this->push_elements();
    }
    return;
  }
  if (children[0]->intersects(fig->get_bounding_box())){
    children[0]->insert(fig);
  }
  if (children[1]->intersects(fig->get_bounding_box())){
    children[1]->insert(fig);
  }
  if (children[2]->intersects(fig->get_bounding_box())){
    children[2]->insert(fig);
  }
  if (children[3]->intersects(fig->get_bounding_box())){
    children[3]->insert(fig);
  }
}

void quad_tree::clear() {
  for (int i = 0; i < CHILDREN_LIMIT; ++i) {
    delete children[i];
    children[i] = nullptr;
  }
  elements.clear();
}

void quad_tree::remove(figure *fig) {
  if (this->is_leaf()) {
    auto it = std::remove(elements.begin(), elements.end(), fig);
    elements.erase(it, elements.end());
    return;
  }
  for (int i = 0; i < CHILDREN_LIMIT; ++i) {
    if (children[i] != nullptr &&
        children[i]->intersects(fig->get_bounding_box())) {
      children[i]->remove(fig);
    }
  }
}

std::unordered_set<figure *>
quad_tree::query(bounding_box bounds,
                 std::unordered_set<figure *> *result,
                 std::vector<bounding_box> *visited_nodes) const {
  std::unordered_set<figure *> local_result;
  std::unordered_set<figure *> &active_result = result ? *result : local_result;

  if (visited_nodes) {
    visited_nodes->push_back(this->bounds);
  }

  if (this->is_leaf()) {
    for (auto element : elements) {
      if (active_result.count(element) > 0)
        continue;
      if (element->inside(bounds.get_center()))
        active_result.insert(element);
    }
  } else {
    for (int i = 0; i < CHILDREN_LIMIT; i++) {
      if (children[i] != nullptr && children[i]->intersects(bounds)) {
        children[i]->query(bounds, &active_result, visited_nodes);
      }
    }
  }

  return active_result;
}

void quad_tree::draw(engine_2d *engine) const {
  std::vector<point> pts = bounds.get_bounding_box();
  if (pts.size() >= 4) {
    color cyan_color(0.0f, 1.0f, 1.0f);
    rasterizer::line::draw(engine, pts[0], pts[1], cyan_color);
    rasterizer::line::draw(engine, pts[1], pts[2], cyan_color);
    rasterizer::line::draw(engine, pts[2], pts[3], cyan_color);
    rasterizer::line::draw(engine, pts[3], pts[0], cyan_color);
  }

  if (!is_leaf()) {
    for (int i = 0; i < CHILDREN_LIMIT; i++) {
      if (children[i] != nullptr) {
        children[i]->draw(engine);
      }
    }
  }
}