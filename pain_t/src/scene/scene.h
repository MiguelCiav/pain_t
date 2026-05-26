#pragma once
#include "../figures/figure.h"
#include "../figures/point.h"
#include <vector>

class scene {
private:
    std::vector<figure*> figures;
    figure* selected_figure = nullptr;

public:
    scene() = default;
    ~scene();

    void add_figure(figure* f);
    const std::vector<figure*>& get_figures() const;
    std::vector<figure*>& get_figures();

    void select(figure* f);
    void deselect();
    figure* get_selected_figure() const;
    figure* query_at(point click) const;
};
