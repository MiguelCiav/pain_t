#include "scene.h"
#include "../engine/engine_2d.h"

scene::~scene() {
    for (figure* fig : figures) {
        delete fig;
    }
}

void scene::add_figure(figure* f) {
    figures.push_back(f);
}

void scene::clear() {
    deselect();
    for (figure* fig : figures) {
        delete fig;
    }
    figures.clear();
}

void scene::reorder_figures(int source_idx, int target_idx) {
    if (source_idx < 0 || source_idx >= static_cast<int>(figures.size()) ||
        target_idx < 0 || target_idx >= static_cast<int>(figures.size()) ||
        source_idx == target_idx) {
        return;
    }
    figure* temp = figures[source_idx];
    figures.erase(figures.begin() + source_idx);
    figures.insert(figures.begin() + target_idx, temp);
}

const std::vector<figure*>& scene::get_figures() const {
    return figures;
}

std::vector<figure*>& scene::get_figures() {
    return figures;
}

void scene::select(figure* f) {
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

figure* scene::get_selected_figure() const {
    return selected_figure;
}

figure* scene::query_at(point click) const {
    // Iterate from topmost to bottommost (reverse order)
    for (auto it = figures.rbegin(); it != figures.rend(); ++it) {
        if ((*it)->inside(click)) {
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
