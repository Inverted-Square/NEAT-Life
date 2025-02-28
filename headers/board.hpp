#pragma once

#include "neat-life_header.hpp"

#include <vector>
#include <utility>
#include <unordered_map>
#include <unordered_set>

class Board
{
public:
    typedef std::pair<const int, const int> coord;

    struct hash_pair
    {
        size_t operator()(const coord &pair) const
        {
            size_t hash1 = std::hash<int>{}(pair.first);
            size_t hash2 = std::hash<int>{}(pair.second);

            return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
        }
    };

private:
    typedef std::unordered_map<coord, int, hash_pair> coord_val;

    std::unordered_set<coord, hash_pair> live_cells;
    coord_val live_cell_activities;
    coord_val dead_cell_neighbours;
    sf::VertexArray vertices{sf::PrimitiveType::Quads};
    const bool infinite;
    unsigned int pattern_width;
    unsigned int pattern_height;

public:
    typedef std::vector<std::string> pattern;

    explicit Board(const bool infinite = false) : infinite(infinite), pattern_width(0u), pattern_height(0u)
    {}

    explicit Board(const pattern &starting_pattern, bool infinite = false);

    void set_pattern(const pattern &starting_pattern);

    double average_activity() const;

private:
    bool cell_stays_live(const coord &row_column);

    static void add_vertex_to(sf::VertexArray &vertex_array, const coord &row_column);

public:
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

    void set_cell(const coord &row_column, bool state = true);

    void update_generation();

private:
    void update_activity(const coord &row_col, int activity, bool set_activity = false);

public:
    inline bool is_infinite() const
    {
        return infinite;
    }

    inline unsigned int get_pattern_width() const
    {
        return pattern_width;
    }

    inline unsigned int get_pattern_height() const
    {
        return pattern_height;
    }

    inline void draw(sf::RenderWindow &window) const
    {
        window.clear();
        window.draw(vertices);
        window.display();
    }
};
