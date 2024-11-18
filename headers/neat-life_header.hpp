#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace win_config
{
    /*
     * For a modified default style, use sf::Style::Titlebar | sf::Style::Close
     * For a fullscreen style, use sf::Style::Fullscreen
     */
    static sf::Uint32 style = sf::Style::Fullscreen;
    const int width = style == sf::Style::Fullscreen ? static_cast<int>(sf::VideoMode::getDesktopMode().width) : 1000;
    const int height = style == sf::Style::Fullscreen ? static_cast<int>(sf::VideoMode::getDesktopMode().height) : 700;

    const unsigned int width_u = static_cast<unsigned int>(width);
    const unsigned int height_u = static_cast<unsigned int>(height);
    const float width_f = static_cast<float>(width);
    const float height_f = static_cast<float>(height);

    static int frame_rate = 0;
}

namespace board_config
{
    static bool infinite = true;

    const int rows = win_config::height / 4;
    const int columns = win_config::width / 4;
    const float column_width = win_config::width_f / static_cast<float>(columns);
    const float row_height = win_config::height_f / static_cast<float>(rows);

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
    const float move_speed = win_config::frame_rate == 0 ? 1 : 10;
    const float movements[4][2] = {{0,  -1},  // up
                                   {0,  1},   // down
                                   {-1, 0},   // left
                                   {1,  0}};  // right
}
