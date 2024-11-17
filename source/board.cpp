#include "board.hpp"

bool Board::cell_stays_live(const coord &row_column)
{
    int live_neighbour_count = 0;
    for (const auto &direction : board_config::directions)
    {
        const int row_index = neg_mod(row_column.first + direction[0], board_config::rows);
        const int column_index = neg_mod(row_column.second + direction[1], board_config::columns);
        const coord row_column_indices = {row_index, column_index};
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

void Board::add_vertex_to(sf::VertexArray& vertex_array, const coord &row_column)
{
    const float x_pos = board_config::column_width * static_cast<float>(row_column.second);
    const float y_pos = board_config::row_height * static_cast<float>(row_column.first);
    vertex_array.append(sf::Vertex({x_pos, y_pos}));
    vertex_array.append(sf::Vertex({x_pos + board_config::column_width, y_pos}));
    vertex_array.append(sf::Vertex({x_pos + board_config::column_width, y_pos + board_config::row_height}));
    vertex_array.append(sf::Vertex({x_pos, y_pos + board_config::row_height}));
}

void Board::set_cell(const coord &row_column)
{
    sf::Vertex vertex({static_cast<float>(row_column.first), static_cast<float>(row_column.second)});
    if (!coord_map_contains(live_cell_activities, row_column))
    {
        live_cell_activities.insert({row_column, 0});
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
            Board::add_vertex_to(next_vertices, live_cell.first);
        }
    }

    for (const auto &dead_cell : dead_cell_neighbours)
    {
        if (dead_cell.second == 3)
        {
            next_live_cell_activities.insert(dead_cell);
            Board::add_vertex_to(next_vertices, dead_cell.first);
        }
    }

    live_cell_activities = std::move(next_live_cell_activities);
    vertices = std::move(next_vertices);
    dead_cell_neighbours.clear();
}

void Board::draw(sf::RenderWindow &window) const
{
    window.clear();
    window.draw(vertices);
    window.display();
}
