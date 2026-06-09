#include "scene_serializer.h"
#include "scene.h"
#include "../figures/figure.h"
#include "../figures/line.h"
#include "../figures/rectangle.h"
#include "../figures/ellipse.h"
#include "../figures/triangle.h"
#include "../figures/bezier.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

bool scene_serializer::save(scene& s, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out.is_open()) {
        return false;
    }
    out << "pain_t v1\n";
    color bg = s.get_background_color();
    out << "background " << bg.r << " " << bg.g << " " << bg.b << "\n";
    
    for (const auto* fig : s.get_figures()) {
        out << "\n";
        out << "figure " << fig->get_type_tag() << "\n";
        out << "z_index " << fig->get_z_index() << "\n";
        out << "bordered " << (fig->is_bordered() ? 1 : 0) << "\n";
        out << "filled " << (fig->is_filled() ? 1 : 0) << "\n";
        color border = fig->get_border_color();
        out << "border_color " << border.r << " " << border.g << " " << border.b << "\n";
        color fill = fig->get_fill_color();
        out << "fill_color " << fill.r << " " << fill.g << " " << fill.b << "\n";
        const auto& cps = fig->get_control_points();
        out << "control_points " << cps.size() << "\n";
        for (const auto& cp : cps) {
            point p = cp.get_position();
            out << "cp " << p.x << " " << p.y << "\n";
        }
    }
    return true;
}

bool scene_serializer::load_into(const std::string& filepath, scene& s, engine_2d* engine) {
    std::ifstream in(filepath);
    if (!in.is_open()) {
        return false;
    }
    std::string line;
    if (!std::getline(in, line)) {
        return false;
    }
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' ')) {
        line.pop_back();
    }
    if (line != "pain_t v1") {
        return false;
    }

    if (!std::getline(in, line)) {
        return false;
    }
    std::stringstream bg_ss(line);
    std::string bg_key;
    float bg_r = 0, bg_g = 0, bg_b = 0;
    if (!(bg_ss >> bg_key >> bg_r >> bg_g >> bg_b) || bg_key != "background") {
        return false;
    }

    s.clear();
    s.set_background_color(color(bg_r, bg_g, bg_b));

    while (std::getline(in, line)) {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
            line.pop_back();
        }
        std::string trimmed = line;
        while (!trimmed.empty() && trimmed.front() == ' ') {
            trimmed.erase(trimmed.begin());
        }
        if (trimmed.empty()) {
            continue;
        }

        std::stringstream ss(trimmed);
        std::string key;
        ss >> key;
        if (key != "figure") {
            continue;
        }

        std::string type_tag;
        ss >> type_tag;

        int z_index = 0;
        color border_color;
        color fill_color;
        int cp_count = 0;
        std::vector<point> points;
        bool has_bordered = false;
        bool bordered_val = true;
        bool has_filled = false;
        bool filled_val = true;

        std::string fig_line;
        while (points.size() < static_cast<size_t>(cp_count) || cp_count == 0) {
            if (!std::getline(in, fig_line)) {
                break;
            }
            while (!fig_line.empty() && (fig_line.back() == '\r' || fig_line.back() == '\n')) {
                fig_line.pop_back();
            }
            std::string fig_trimmed = fig_line;
            while (!fig_trimmed.empty() && fig_trimmed.front() == ' ') {
                fig_trimmed.erase(fig_trimmed.begin());
            }
            if (fig_trimmed.empty()) {
                continue;
            }

            std::stringstream fig_ss(fig_trimmed);
            std::string fig_key;
            fig_ss >> fig_key;
            if (fig_key == "z_index") {
                fig_ss >> z_index;
            } else if (fig_key == "bordered") {
                int val = 1;
                fig_ss >> val;
                bordered_val = (val != 0);
                has_bordered = true;
            } else if (fig_key == "filled") {
                int val = 1;
                fig_ss >> val;
                filled_val = (val != 0);
                has_filled = true;
            } else if (fig_key == "border_color") {
                fig_ss >> border_color.r >> border_color.g >> border_color.b;
            } else if (fig_key == "fill_color") {
                fig_ss >> fill_color.r >> fill_color.g >> fill_color.b;
            } else if (fig_key == "control_points") {
                fig_ss >> cp_count;
            } else if (fig_key == "cp") {
                double x = 0, y = 0;
                fig_ss >> x >> y;
                points.push_back(point(x, y));
            } else if (fig_key == "figure") {
                break;
            }
        }

        figure* fig = nullptr;
        if (type_tag == "line" && points.size() >= 2) {
            fig = new ::line(points[0], points[1], border_color, engine);
        } else if (type_tag == "rectangle") {
            fig = new rectangle(points, border_color, fill_color, true, engine);
        } else if (type_tag == "ellipse") {
            fig = new ellipse(points, border_color, fill_color, true, engine);
        } else if (type_tag == "triangle") {
            fig = new triangle(points, border_color, fill_color, true, engine);
        } else if (type_tag == "bezier") {
            fig = new bezier(points, border_color, engine);
        }

        if (fig) {
            fig->set_z_index(z_index);
            fig->set_border_color(border_color);
            if (has_bordered) {
                fig->set_bordered(bordered_val);
            }
            if (fig->can_fill()) {
                fig->set_fill_color(fill_color);
                if (has_filled) {
                    fig->set_filled(filled_val);
                }
            }
            s.add_figure(fig);
        }
    }
    return true;
}
