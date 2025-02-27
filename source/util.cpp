#include "util.hpp"

void setup_view(sf::View &view, const bool is_infinite, const int pattern_width, const int pattern_height)
{
    const float half_width = win_config::width_f / 2.0f;
    const float half_height = win_config::height_f / 2.0f;
    if (is_infinite)
    {
        const float half_pattern_rows = static_cast<float>(pattern_height) / 2.0f;
        const float half_pattern_columns = static_cast<float>(pattern_width) / 2.0f;
        const float half_pattern_width = board_config::cell_length * half_pattern_columns;
        const float half_pattern_height = board_config::cell_length * half_pattern_rows;
        view.move(-half_width + half_pattern_width, -half_height + half_pattern_height);
    }
    else
    {
        const float finite_width = board_config::cell_length * board_config::columns;
        const float finite_height = board_config::cell_length * board_config::rows;
        const float max_finite_length = max(finite_width, finite_height);
        const float min_window_length = min(win_config::width_f, win_config::height_f);
        view.move(-half_width + finite_width / 2.0f, -half_height + finite_height / 2.0f);
        view.zoom(max_finite_length / min_window_length);
    }
}
