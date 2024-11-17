#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace win_config
{
    const sf::Uint32 style = sf::Style::Default;
    const int width = style == sf::Style::Default ? 1000 : static_cast<int>(sf::VideoMode::getDesktopMode().width);
    const int height = style == sf::Style::Default ? 700 : static_cast<int>(sf::VideoMode::getDesktopMode().height);
    const int frame_rate = 0;
}

namespace board_config
{
    const int rows = win_config::height / 2;
    const int columns = win_config::width / 2;
    const float column_width = static_cast<float>(win_config::width) / static_cast<float>(columns);
    const float row_height = static_cast<float>(win_config::height) / static_cast<float>(rows);
    const int directions[8][2] = {{-1, -1},
                                  {-1, 0},
                                  {-1, 1},
                                  {0,  -1},
                                  {0,  1},
                                  {1,  -1},
                                  {1,  0},
                                  {1,  1}};
}

namespace events_config
{
    const float movements[4][2] = {{0,  -1},  // up
                                   {0,  1},   // down
                                   {-1, 0},   // left
                                   {1,  0}};  // right
}
