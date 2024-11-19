#include "board.hpp"

Board::Board(const Board::pattern &starting_pattern, const bool infinite) : Board(infinite)
{
    if (starting_pattern.size() > board_config::rows && !infinite)
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
            next_live_cells.clear();
            next_dead_cells.clear();
            return;
        }

        pattern_width = row_string.size() > pattern_width ? row_string.size() : pattern_width;
        const int x_offset = board_config::columns / 2 - static_cast<int>(row_string.size()) / 2;
        int column = 0;
        for (const char cell : row_string)
        {
            if (cell == 'O')
            {
                update_cell({row + y_offset * !infinite, column + x_offset * !infinite}, true);
            }
            ++column;
        }

        ++row;
    }

    update_on_differences();
}

void Board::update_cell(const Board::coord &row_column, const bool is_birth)
{
    if (is_birth)
    {
        next_dead_cells.erase(row_column);
        updated_dead.erase(row_column);
        removed_dead.insert(row_column);
        next_live_cells[row_column] = 0;
        updated_live[row_column] = 0;
    }
    else
    {
        if (next_live_cells[row_column] != 0)
        {
            next_dead_cells[row_column] = next_live_cells[row_column];
            updated_dead[row_column] = next_live_cells[row_column];
        }

        next_live_cells.erase(row_column);
        updated_live.erase(row_column);
        removed_live.insert(row_column);
    }

    for (const auto &direction : board_config::directions)
    {
        int row_index = row_column.first + direction[0];
        int column_index = row_column.second + direction[1];
        if (!infinite)
        {
            row_index = Board::neg_mod(row_index, board_config::rows);
            column_index = Board::neg_mod(column_index, board_config::columns);
        }

        const coord neighbour_pos = {row_index, column_index};
        if (is_birth)
        {
            update_on_birth(row_column, neighbour_pos);
        }
        else
        {
            update_on_death(neighbour_pos);
        }
    }
}

void Board::update_on_birth(const Board::coord &row_column, const Board::coord &neighbour_pos)
{
    if (next_live_cells.count(neighbour_pos) != 0)
    {
        updated_live[row_column] = ++next_live_cells[row_column];
        updated_live[neighbour_pos] = ++next_live_cells[neighbour_pos];
        return;
    }

    if (next_dead_cells.count(neighbour_pos) != 0)
    {
        updated_dead[neighbour_pos] = ++next_dead_cells[neighbour_pos];
    }
    else
    {
        next_dead_cells[neighbour_pos] = 1;
        updated_dead[neighbour_pos] = 1;
    }
}

void Board::update_on_death(const Board::coord &neighbour_pos)
{
    if (next_live_cells.count(neighbour_pos) != 0)
    {
        updated_live[neighbour_pos] = --next_live_cells[neighbour_pos];
        return;
    }

    updated_dead[neighbour_pos] = --next_dead_cells[neighbour_pos];
    if (next_dead_cells[neighbour_pos] == 0)
    {
        next_dead_cells.erase(neighbour_pos);
        updated_dead.erase(neighbour_pos);
        removed_dead.insert(neighbour_pos);
    }
}

void Board::update_on_differences()
{
    for (const auto &rm_live : removed_live)
    {
        live_cells.erase(rm_live);
    }

    for (const auto &rm_dead : removed_dead)
    {
        dead_cells.erase(rm_dead);
    }

    for (const auto &up_live : updated_live)
    {
        live_cells[up_live.first] = up_live.second;
    }

    for (const auto &up_dead : updated_dead)
    {
        dead_cells[up_dead.first] = up_dead.second;
    }

    removed_live.clear();
    removed_dead.clear();
    updated_live.clear();
    updated_dead.clear();
}

void Board::update_generation()
{
    for (const auto &live_cell : live_cells)
    {
        if (live_cell.second != 2 && live_cell.second != 3)
        {
            update_cell(live_cell.first, false);
        }
    }

    for (const auto &dead_cell : dead_cells)
    {
        if (dead_cell.second == 3)
        {
            update_cell(dead_cell.first, true);
        }
    }

    update_on_differences();
}

void Board::draw(sf::RenderWindow &window) const
{
    window.clear();
    sf::VertexArray vertex_array{sf::PrimitiveType::Quads, 4 * live_cells.size()};
    for (const auto &cell : live_cells)
    {
        coord cell_pos = cell.first;
        const auto x_pos = static_cast<float>(cell_pos.second);
        const auto y_pos = static_cast<float>(cell_pos.first);
        vertex_array.append(sf::Vertex({x_pos, y_pos}));
        vertex_array.append(sf::Vertex({x_pos + 1.0f, y_pos}));
        vertex_array.append(sf::Vertex({x_pos + 1.0f, y_pos + 1.0f}));
        vertex_array.append(sf::Vertex({x_pos, y_pos + 1.0f}));
    }

    window.draw(vertex_array);
    window.display();
}
