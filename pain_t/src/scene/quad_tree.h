#pragma once

#include "figures/bounding_box.h"
#include "figures/figure.h"
#include <unordered_set>
#include <vector>

const int CHILDREN_LIMIT = 4;
const int ELEMENTS_LIMIT = 8;
const int LEVELS_LIMIT = 8;

class engine_2d;

class quad_tree {
private:
  bounding_box bounds;
  std::vector<figure *> elements;
  quad_tree *children[CHILDREN_LIMIT] = {nullptr};
  int level = 0;
  bool is_leaf() const { return children[0] == nullptr; }
  void create_children();
  void push_elements();

public:
  quad_tree(bounding_box bounds, int level) : bounds(bounds), level(level) {};
  ~quad_tree() {
    for (int i = 0; i < CHILDREN_LIMIT; ++i) {
      delete children[i];
    }
  }

  void clear();
  void insert(figure *fig);
  void remove(figure *fig);
  bool intersects(bounding_box box) const;
  std::unordered_set<figure *>
  query(bounding_box bounds,
        std::unordered_set<figure *> *result = nullptr,
        std::vector<bounding_box> *visited_nodes = nullptr) const;
  void draw(engine_2d *engine) const;
};