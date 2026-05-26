#pragma once
#include "../figures/figure.h"
#include "../figures/point.h"
#include <vector>

class scene {
private:
    std::vector<figure*> figures;
    figure* selected_figure = nullptr;
    color background_color = color(0.85f, 0.85f, 0.85f);
    color active_border_color = color(0.0f, 0.0f, 0.0f);
    color active_fill_color = color(1.0f, 1.0f, 1.0f);

public:
    scene() = default;
    ~scene();

    void add_figure(figure* f);
    void clear();
    const std::vector<figure*>& get_figures() const;
    std::vector<figure*>& get_figures();

    void select(figure* f);
    void deselect();
    figure* get_selected_figure() const;
    figure* query_at(point click) const;

    // Drawing
    void draw_all(engine_2d *engine) const;

    // Document Color State
    color get_background_color() const { return background_color; }
    void set_background_color(color c) { background_color = c; }
    color get_active_border_color() const { return active_border_color; }
    void set_active_border_color(color c) { active_border_color = c; }
    color get_active_fill_color() const { return active_fill_color; }
    void set_active_fill_color(color c) { active_fill_color = c; }
};
