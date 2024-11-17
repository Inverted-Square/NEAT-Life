#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>
#include <map>

class Board
{
    const int ROWS;
    const int COLUMNS;
    std::map<std::pair<int, int>, int> live_cell_activities;
    std::map<const std::pair<const int, const int>, int> dead_cell_neighbours;
    const int DIRECTIONS[8][2] = {{-1, -1},
                                  {-1, 0},
                                  {-1, 1},
                                  {0,  -1},
                                  {0,  1},
                                  {1,  -1},
                                  {1,  0},
                                  {1,  1}};

    template<class CoordMap>
    static inline bool coord_map_contains(const CoordMap &coord_map, const std::pair<int, int> &row_column)
    {
        return coord_map.find(row_column) != coord_map.end();
    }

    bool cell_stays_live(const std::pair<const int, const int> &row_column);

    static inline int neg_mod(int n, int m)
    {
        return (m + (n % m)) % m;
    }

public:
    Board(const int rows, const int columns) : ROWS(rows), COLUMNS(columns)
    {}

    void set_cell(const std::pair<int, int> &row_column, bool state);

    void update_generation();

    void draw(sf::RenderWindow &window) const;
};
