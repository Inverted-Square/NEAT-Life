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
    const int rows = 100;
    const int columns = 600;
    const float cell_length = 10.0f;
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

namespace neat_config
{
    const int pattern_width = 3;
    const int pattern_height = 90;
    const int target_cells = 14;
    const int area = pattern_width * pattern_height;
    const double max_distance = area - target_cells + 1;
    const int num_input = pattern_width * pattern_height + 3;
    const int num_output = 2;
    const std::vector<int> layer_counts = {num_input, num_output};
    const int population_size = 25;
    const int num_gen = 150;
    const bool debug_mode = false;
    const int tournament_size = 3;
    const double weight_prob = 0.8;
    const double node_prob = 0.001;
    const double conn_prob = 0.0001;
    const int max_steps = 10000;
    const int max_same = 160;
    const char fitness_function = 'b';  // 'm' - methuselah, 'c' - complexity, 'b' - both
    const bool infinite_board = false;
    const char infinite_axis = 'x';  // 'x' to make it infinitely wide, 'y' to make it infinitely tall and '-' to do leave the board completely finite
}

static std::random_device random_device;
static std::mt19937 gen;  // use random_device to seed when you're not testing
static std::normal_distribution<double> weight_distribution(0, sqrt(2.0 / neat_config::num_input));  // TODO: remember to talk about this in the final report
static std::uniform_real_distribution<double> rand_prob(0, 1);
static std::uniform_real_distribution<double> mutate_power(0.5, 1.5);
