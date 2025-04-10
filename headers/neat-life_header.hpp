#pragma once

#include <iostream>
#include <algorithm>
#include <random>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace win_config
{
    /*
     * For a modified default style, use sf::Style::Titlebar | sf::Style::Close
     * For a fullscreen style, use sf::Style::Fullscreen
     */
    static sf::Uint32 style = sf::Style::Titlebar | sf::Style::Close;
    const int width = style == sf::Style::Fullscreen ? static_cast<int>(sf::VideoMode::getDesktopMode().width) : 1000;  // don't change
    const int height = style == sf::Style::Fullscreen ? static_cast<int>(sf::VideoMode::getDesktopMode().height) : 700;  // don't change

    const unsigned int width_u = static_cast<unsigned int>(width);  // don't change
    const unsigned int height_u = static_cast<unsigned int>(height);  // don't change
    const float width_f = static_cast<float>(width);  // don't change
    const float height_f = static_cast<float>(height);  // don't change

    static int frame_rate = 0;  // 0 to make it run as fast as possible (any other num > 0 will act as you'd expect for the frame rate)
}

namespace board_config
{
    const int rows = 200;  // >= 1
    const int columns = 200;  // >= 1
    const float cell_length = 10.0f;  // // don't change
    const int directions[8][2] = {{-1, -1},  // don't change
                                  {-1, 0},
                                  {-1, 1},
                                  {0,  -1},
                                  {0,  1},
                                  {1,  -1},
                                  {1,  0},
                                  {1,  1}};
}

namespace events_config  // don't change
{
    const float move_speed = win_config::frame_rate == 0 ? 1 : 10;
    const float movements[4][2] = {{0,  -1},  // up
                                   {0,  1},   // down
                                   {-1, 0},   // left
                                   {1,  0}};  // right
}

namespace neat_config
{
    const int pattern_width = 10;  // >= 1
    const int pattern_height = 10;  // >= 1
    const int target_cells = 13;  // >= 0
    const int area = pattern_width * pattern_height;  // don't change
    const double max_distance = area - target_cells + 1;  // don't change
    const int num_input = pattern_width * pattern_height + 2;  // don't change
    const int num_output = 2;  // don't change
    const std::vector<int> layer_counts = {num_input, num_output};  // don't change
    const int population_size = 100;  // >= 1
    const int num_gen = 150;  // >= 1
    const int tournament_size = 3;  // >= 2
    const double weight_prob = 0.8;  // between 0 and 1 (high prob recommended)
    const double node_prob = 0.001;  // between 0 and 1 (low prob recommended)
    const double conn_prob = 0.0001;  // between 0 and 1 (low prob recommended)
    const int max_steps = 20000;  // >= 0 (at least 10000 recommended though)
    const int max_same = 150;  // don't change
    const char fitness_function = 'c';  // 'm' - methuselah, 'c' - complexity, 'b' - m and c, 'l' - max num live
    const bool infinite_board = true;
    const char infinite_axis = '-';  // 'x' to make it infinitely wide, 'y' to make it infinitely tall and '-' to do leave the board completely finite
    const bool random_element = true;
}

static std::random_device random_device;
static std::mt19937 gen;  // use random_device to seed when you're not testing
static std::normal_distribution<double> weight_distribution(0, sqrt(2.0 / neat_config::num_input));  // don't change
static std::uniform_real_distribution<double> rand_prob(0, 1);  // don't change
static std::uniform_real_distribution<double> mutate_power(0.5, 1.5);  // don't change
