#include "board.hpp"

Board::Board(const Board::pattern &starting_pattern, const bool infinite) : Board(infinite)
{
    if (starting_pattern.size() > board_config::rows)  // TODO: shouldn't this also say '&& !infinite'
    {
        std::cout << "Pattern too tall in finite area. Increase board_config::rows in 'neat-life_header.hpp'.\n";
        return;
    }

    pattern_height = starting_pattern.size();
    const int y_offset = board_config::rows / 2 - static_cast<int>(starting_pattern.size()) / 2;
    int row = 0;
    for (const auto &row_string : starting_pattern)
    {
        if (row_string.size() > board_config::columns && !infinite)
        {
            std::cout << "Pattern too wide in finite area. Increase board_config::columns in 'neat-life_header.hpp'.\n";
            live_cell_activities.clear();
            vertices.clear();
            return;
        }

        pattern_width = row_string.size() > pattern_width ? row_string.size() : pattern_width;
        const int x_offset = board_config::columns / 2 - static_cast<int>(row_string.size()) / 2;
        int column = 0;
        for (const char cell : row_string)
        {
            set_cell({row + y_offset * !infinite, column + x_offset * !infinite}, cell == 'O');
            ++column;
        }

        ++row;
    }
}

double Board::average_activity() const
{
    double activity_sum = 0;
    for (const auto &[coord, activity] : live_cell_activities)
    {
        if (activity == -1)
        {
            continue;
        }

        activity_sum += activity;
    }

    return live_cell_activities.empty() ? 0 : activity_sum / static_cast<double>(live_cell_activities.size());
}

bool Board::cell_stays_live(const coord &row_column)
{
    int live_neighbour_count = 0;
    for (const auto &direction : board_config::directions)
    {
        int row_index = row_column.first + direction[0];
        int column_index = row_column.second + direction[1];
        if (!infinite)
        {
            row_index = Board::neg_mod(row_index, board_config::rows);
            column_index = Board::neg_mod(column_index, board_config::columns);
        }

        const coord row_column_indices = {row_index, column_index};
        if (live_cell_activities.count({row_index, column_index}) != 0)
        {
            ++live_neighbour_count;
            continue;
        }

        if (dead_cell_neighbours.count(row_column_indices) != 0)
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

void Board::add_vertex_to(sf::VertexArray &vertex_array, const coord &row_column)
{
    const float x_pos = board_config::cell_length * static_cast<float>(row_column.second);
    const float y_pos = board_config::cell_length * static_cast<float>(row_column.first);
    vertex_array.append(sf::Vertex({x_pos, y_pos}));
    vertex_array.append(sf::Vertex({x_pos + board_config::cell_length, y_pos}));
    vertex_array.append(sf::Vertex({x_pos + board_config::cell_length, y_pos + board_config::cell_length}));
    vertex_array.append(sf::Vertex({x_pos, y_pos + board_config::cell_length}));
}

void Board::set_cell(const coord &row_column, const bool state)
{
    if (live_cell_activities.count(row_column) == 0 && state)
    {
        live_cell_activities.insert({row_column, -1});
        sf::Vertex vertex({static_cast<float>(row_column.first), static_cast<float>(row_column.second)});
        Board::add_vertex_to(vertices, row_column);
    }
}

void Board::update_generation()
{
    coord_val next_live_cell_activities;
    sf::VertexArray next_vertices{sf::PrimitiveType::Quads};
    for (const auto &live_cell : live_cell_activities)
    {
        if (cell_stays_live(live_cell.first))
        {
            next_live_cell_activities.insert(live_cell);
            update_activity(live_cell.first, -1);
            Board::add_vertex_to(next_vertices, live_cell.first);
        }
        else
        {
            update_activity(live_cell.first, 0, true);
        }
    }

    for (const auto &dead_cell : dead_cell_neighbours)
    {
        if (dead_cell.second == 3)
        {
            next_live_cell_activities.insert(dead_cell);
            update_activity(dead_cell.first, 50);
            Board::add_vertex_to(next_vertices, dead_cell.first);
        }
        else
        {
            update_activity(dead_cell.first, 0, true);
        }
    }

    live_cell_activities = std::move(next_live_cell_activities);
    vertices = std::move(next_vertices);
    dead_cell_neighbours.clear();
}

void Board::update_activity(const coord &row_col, const int activity, const bool set_activity)
{
    if (live_cell_activities.count(row_col) != 0)
    {
        if (set_activity)
        {
            live_cell_activities[row_col] = activity;
        }
        else
        {
            live_cell_activities[row_col] += activity;
        }
    }
    else
    {
        live_cell_activities[row_col] = activity;
    }

    if (live_cell_activities[row_col] <= 0)
    {
        live_cell_activities[row_col] = -1;
    }
}
