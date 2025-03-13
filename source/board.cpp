#include "board.hpp"

Board::Board(const Board::pattern &starting_pattern, const bool infinite) : Board(infinite)
{
    set_pattern(starting_pattern);
}

void Board::set_pattern(const Board::pattern &starting_pattern)
{
    live_cells.clear();
    live_cell_activities.clear();
    dead_cell_neighbours.clear();
    vertices.clear();
    if (starting_pattern.size() > board_config::rows && !infinite && neat_config::infinite_axis != 'y')
    {
        std::cout << "Pattern too tall in finite area. Increase board_config::rows in 'neat-life_header.hpp'.\n";
        return;
    }

    pattern_height = starting_pattern.size();
    const int y_offset = board_config::rows / 2 - static_cast<int>(starting_pattern.size()) / 2;
    int row = 0;
    for (const auto &row_string : starting_pattern)
    {
        if (row_string.size() > board_config::columns && !infinite && neat_config::infinite_axis != 'x')
        {
            std::cout << "Pattern too wide in finite area. Increase board_config::columns in 'neat-life_header.hpp'.\n";
            live_cells.clear();
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
        activity_sum += activity;
    }

    return live_cell_activities.empty() ? 0 : activity_sum / static_cast<double>(live_cell_activities.size());
}

int Board::get_complexity() const
{
    int width = 0;
    int height = 0;
    get_min_bounding_box(width, height);
    const int width_bits = get_min_bits(width);
    const int height_bits = get_min_bits(height);
    const int num_live = static_cast<int>(live_cells.size());
    const int num_live_bits = get_min_bits(num_live);
    const int alpha = width_bits + height_bits + width * height;
    const int beta = width_bits + height_bits + num_live_bits +
            static_cast<int>(ceil(log2Comb(width * height, num_live)));
    return 1 + (alpha < beta ? alpha : beta);
}

void Board::get_min_bounding_box(int &width, int &height) const
{
    width = 0;
    height = 0;
    if (live_cells.empty())
    {
        return;
    }

    int min_row = INT32_MAX;
    int max_row = INT32_MIN;
    int min_column = INT32_MAX;
    int max_column = INT32_MIN;
    for (const auto &[row, col] : live_cells)
    {
        min_row = row < min_row ? row : min_row;
        max_row = row > max_row ? row : max_row;
        min_column = col < min_column ? col : min_column;
        max_column = col > max_column ? col : max_column;
    }

    width = max_column - min_column + 1;
    height = max_row - min_row + 1;
}

int Board::get_min_bits(const int value)
{
    return value == 0 ? 0 : static_cast<int>(ceil(log2(value + 1) + log2(value))) + 1;
}

double Board::log2Comb(const double n, const double r)
{
    if (r == 0)
    {
        return 1;
    }

    if (r > n / 2)
    {
        return log2Comb(n, n - r);
    }

    double res = 0;
    for (unsigned long long k = 1; static_cast<double>(k) <= r; ++k)
    {
        res += log2(n - static_cast<double>(k) + 1);
        res -= log2(static_cast<double>(k));
    }

    return res;
}

bool Board::cell_stays_live(const coord &row_column)
{
    int live_neighbour_count = 0;
    for (const auto &direction : board_config::directions)
    {
        int row_index = row_column.first + direction[0];
        int column_index = row_column.second + direction[1];
        if (!infinite && neat_config::infinite_axis != 'y')
        {
            row_index = Board::neg_mod(row_index, board_config::rows);
        }

        if (!infinite && neat_config::infinite_axis != 'x')
        {
            column_index = Board::neg_mod(column_index, board_config::columns);
        }

        const coord row_column_indices = {row_index, column_index};
        if (live_cells.count({row_index, column_index}) != 0)
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
    if (live_cells.count(row_column) == 0 && state)
    {
        live_cells.insert(row_column);
        sf::Vertex vertex({static_cast<float>(row_column.first), static_cast<float>(row_column.second)});
        Board::add_vertex_to(vertices, row_column);
    }
}

void Board::update_generation()
{
    std::unordered_set<coord, hash_pair> next_live_cells;
    sf::VertexArray next_vertices{sf::PrimitiveType::Quads};
    for (const auto &live_cell : live_cells)
    {
        if (cell_stays_live(live_cell))
        {
            next_live_cells.insert(live_cell);
            update_activity(live_cell, -1);
            Board::add_vertex_to(next_vertices, live_cell);
        }
        else
        {
            update_activity(live_cell, 0, true);
        }
    }

    for (const auto &dead_cell : dead_cell_neighbours)
    {
        if (dead_cell.second == 3)
        {
            next_live_cells.insert(dead_cell.first);
            update_activity(dead_cell.first, 50);
            Board::add_vertex_to(next_vertices, dead_cell.first);
        }
        else
        {
            update_activity(dead_cell.first, 0, true);
        }
    }

    live_cells = std::move(next_live_cells);
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
        live_cell_activities.erase(row_col);
    }
}
