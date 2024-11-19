#pragma once

#include <iostream>

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

    const int frame_rate = 0;
}

namespace board_config
{
    const int rows = 400;
    const int columns = 400;

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
    const float move_speed = 5;
    const float movements[4][2] = {{0,  -1},  // up
                                   {0,  1},   // down
                                   {-1, 0},   // left
                                   {1,  0}};  // right
}
