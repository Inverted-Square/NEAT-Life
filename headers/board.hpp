#pragma once

#include <vector>
#include <map>
#include <array>
#include <algorithm>
#include <SFML/Graphics.hpp>

class Board
{
    const int ROWS;
    const int COLUMNS;
    std::vector<std::vector<bool>> cells;
    std::vector<std::array<int, 2>> live_cells;
    std::map<const std::array<int, 2>, int> dead_cell_neighbours;

    [[nodiscard]] std::vector<std::vector<bool>> setup_cells() const;

    bool cell_stays_live(int row, int column);

    static inline int neg_mod(int a, int b);

public:
    Board(const int rows, const int columns) : ROWS(rows), COLUMNS(columns), cells(setup_cells())
    {}

    void set_cell(const std::array<int, 2>& row_column, bool state);

    void update_generation();

    void draw(sf::RenderWindow& window) const;
};
