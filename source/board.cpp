#include "board.hpp"

std::vector<std::vector<bool>> Board::setup_cells() const
{
    std::vector<std::vector<bool>> new_cells(Board::ROWS);
    for (int row = 0; row < Board::ROWS; ++row)
    {
        new_cells[row] = std::vector<bool>(Board::COLUMNS);
    }

    return new_cells;
}

bool Board::cell_stays_live(const int row, const int column)
{
    int live_neighbour_count = 0;
    const int directions[8][2] = {{-1, -1},
                                  {-1, 0},
                                  {-1, 1},
                                  {0,  -1},
                                  {0,  1},
                                  {1,  -1},
                                  {1,  0},
                                  {1,  1}};
    for (const auto& direction : directions)
    {
        const int row_index = neg_mod(row + direction[0], static_cast<int>(cells.size()));
        const int column_index = neg_mod(column + direction[1], static_cast<int>(cells[row_index].size()));
        if (cells[row_index][column_index])
        {
            ++live_neighbour_count;
            continue;
        }

        const std::array<int, 2> row_column_indices = {row_index, column_index};
        if (dead_cell_neighbours.find(row_column_indices) != dead_cell_neighbours.end())  // the key was found
        {
            ++dead_cell_neighbours[row_column_indices];
        }
        else  // the key was not found
        {
            dead_cell_neighbours.insert({row_column_indices, 1});
        }
    }

    return live_neighbour_count == 2 || live_neighbour_count == 3;
}

void Board::set_cell(const std::array<int, 2>& row_column, bool state)
{
    int row = row_column[0];
    int column = row_column[1];
    if (!cells[row][column] && state)
    {
        live_cells.push_back(row_column);
    }
    else if (cells[row][column] && !state)
    {
        live_cells.erase(std::remove(live_cells.begin(), live_cells.end(), row_column), live_cells.end());
    }

    cells[row][column] = state;
}

void Board::update_generation()
{
    std::vector<std::vector<bool>> next_generation = setup_cells();
    std::vector<std::array<int, 2>> next_live_cells;

    for (const auto live_cell : live_cells)
    {
        if (cell_stays_live(live_cell[0], live_cell[1]))
        {
            next_generation[live_cell[0]][live_cell[1]] = true;
            next_live_cells.push_back(live_cell);
        }
    }

    for (const auto dead_cell : dead_cell_neighbours)
    {
        if (dead_cell.second == 3)
        {
            next_generation[dead_cell.first[0]][dead_cell.first[1]] = true;
            next_live_cells.push_back(dead_cell.first);
        }
    }

    cells = next_generation;
    live_cells = next_live_cells;
    dead_cell_neighbours.clear();
}

void Board::draw(sf::RenderWindow& window) const
{
    window.clear();
    const float column_width = static_cast<float>(window.getSize().x) / static_cast<float>(Board::COLUMNS);
    const float row_height = static_cast<float>(window.getSize().y) / static_cast<float>(Board::ROWS);
    for (const auto& live_cell : live_cells)
    {
        const float x_pos = column_width * (static_cast<float>(live_cell[1]) + 0.5f);
        const float y_pos = row_height * (static_cast<float>(live_cell[0]) + 0.5f);
        sf::CircleShape shape{column_width / 2.0f};
        shape.setPosition(x_pos, y_pos);
        window.draw(shape);
    }

    window.display();
}

int Board::neg_mod(int a, int b)
{
    return (b + (a % b)) % b;
}
