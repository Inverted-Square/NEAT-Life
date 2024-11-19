#pragma once

#include "neat-life_header.hpp"

#include <vector>
#include <utility>
#include <unordered_map>
#include <unordered_set>

class Board
{
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

    typedef std::unordered_map<coord, int, hash_pair> coord_val;
    typedef std::unordered_set<coord, hash_pair> coords;

    coord_val live_cells;
    coord_val dead_cells;
    coord_val next_live_cells;
    coord_val next_dead_cells;

    coords removed_live;
    coords removed_dead;
    coord_val updated_live;
    coord_val updated_dead;

    const bool infinite;
    unsigned int pattern_width;
    unsigned int pattern_height;

public:
    typedef std::vector<std::string> pattern;

    explicit Board(const bool infinite = false) : infinite(infinite), pattern_width(0u), pattern_height(0u)
    {}

    explicit Board(const pattern &starting_pattern, bool infinite = false);

private:
    void update_cell(const coord &row_column, bool is_birth);

    void update_on_birth(const coord &row_column, const coord &neighbour_pos);

    void update_on_death(const coord &neighbour_pos);

    void update_on_differences();

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

public:
    void update_generation();

    void draw(sf::RenderWindow &window) const;

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
};
