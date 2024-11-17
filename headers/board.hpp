#pragma once

#include "neat-life_header.hpp"

#include <vector>
#include <utility>
#include <map>

class Board
{
    typedef std::pair<const int, const int> coord;
    typedef std::map<coord, int> coord_val;

    coord_val live_cell_activities;
    coord_val dead_cell_neighbours;
    sf::VertexArray vertices{sf::PrimitiveType::Quads};

    static inline bool coord_map_contains(const coord_val &coord_map, const coord &row_column)
    {
        return coord_map.find(row_column) != coord_map.end();
    }

    bool cell_stays_live(const coord &row_column);

    static inline int neg_mod(int n, int m)
    {
        return (m + (n % m)) % m;
    }

    static void add_vertex_to(sf::VertexArray &vertex_array, const coord &row_column);

public:
    void set_cell(const coord &row_column);

    void update_generation();

    void draw(sf::RenderWindow &window) const;
};
