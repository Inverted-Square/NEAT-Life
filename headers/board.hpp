#pragma once

#include "neat-life_header.hpp"

#include <vector>
#include <utility>
#include <unordered_map>

class Board
{
    typedef std::pair<int, int> coord;

    struct hash_pair
    {
        size_t operator()(const coord &pair) const
        {
            size_t hash1 = std::hash<int>{}(pair.first);
            size_t hash2 = std::hash<int>{}(pair.second);

            return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
        }
    };

    typedef std::unordered_map<coord, int, hash_pair> coord_val;

    coord_val live_cell_activities;
    coord_val dead_cell_neighbours;
    sf::VertexArray vertices{sf::PrimitiveType::Quads};
    const bool infinite;

    bool cell_stays_live(const coord &row_column);

    /**
     * Handles negative values of n in n % m in the same way Python does.
     * @param n The number to get the modulus of.
     * @param m The divisor.
     * @return n % m
     */
    static inline int neg_mod(int n, int m)
    {
        return (n % m + m) % m;
    }

    static void add_vertex_to(sf::VertexArray &vertex_array, const coord &row_column);

public:
    explicit Board(const bool infinite = false) : infinite(infinite)
    {}

    void set_cell(const coord &row_column);

    void update_generation();

    inline void draw(sf::RenderWindow &window) const
    {
        window.clear();
        window.draw(vertices);
        window.display();
    }
};
