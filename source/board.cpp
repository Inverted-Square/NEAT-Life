#include "board.hpp"

bool Board::cell_stays_live(const std::pair<const int, const int> &row_column)
{
    int live_neighbour_count = 0;
    for (const auto &direction : DIRECTIONS)
    {
        const int row_index = neg_mod(row_column.first + direction[0], ROWS);
        const int column_index = neg_mod(row_column.second + direction[1], COLUMNS);
        const std::pair<const int, const int> row_column_indices = {row_index, column_index};
        if (coord_map_contains(live_cell_activities, {row_index, column_index}))
        {
            ++live_neighbour_count;
            continue;
        }

        if (coord_map_contains(dead_cell_neighbours, row_column_indices))
        {
            ++dead_cell_neighbours[row_column_indices];
        }
        else
        {
            dead_cell_neighbours.insert({row_column_indices, 1});
        }
    }

    return live_neighbour_count == 2 || live_neighbour_count == 3;
}

void Board::set_cell(const std::pair<int, int> &row_column, const bool state)
{
    if (!coord_map_contains(live_cell_activities, row_column) && state)
    {
        live_cell_activities.insert({row_column, 0});
    }
    else if (coord_map_contains(live_cell_activities, row_column) && !state)
    {
        live_cell_activities.erase(row_column);
    }
}

void Board::update_generation()
{
    std::map<std::pair<int, int>, int> next_live_cell_activities;
    for (const auto &live_cell : live_cell_activities)
    {
        if (cell_stays_live(live_cell.first))
        {
            next_live_cell_activities.insert(live_cell);
        }
    }

    for (const auto &dead_cell : dead_cell_neighbours)
    {
        if (dead_cell.second == 3)
        {
            next_live_cell_activities.insert(dead_cell);
        }
    }

    live_cell_activities = std::move(next_live_cell_activities);
    dead_cell_neighbours.clear();
}

void Board::draw(sf::RenderWindow &window) const
{
    window.clear();
    const float column_width = static_cast<float>(window.getSize().x) / static_cast<float>(Board::COLUMNS);
    const float row_height = static_cast<float>(window.getSize().y) / static_cast<float>(Board::ROWS);
    for (const auto &live_cell : live_cell_activities)
    {
        const auto &row_column = live_cell.first;
        const float x_pos = column_width * (static_cast<float>(row_column.second) + 0.5f);
        const float y_pos = row_height * (static_cast<float>(row_column.first) + 0.5f);
        sf::CircleShape shape{column_width / 2.0f};
        shape.setPosition(x_pos, y_pos);
        window.draw(shape);
    }

    window.display();
}
