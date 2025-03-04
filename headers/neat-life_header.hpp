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
    const int rows = 29;
    const int columns = 32;
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
    const int num_input = 4;
    const int num_output = 2;
    const std::vector<int> layer_counts = {num_input, num_output};
    const int population_size = 100;
    const int num_gen = 100;
    const bool debug_mode = false;
    const int tournament_size = 3;
    const double weight_prob = 0.8;
    const double node_prob = 0.01;
    const double conn_prob = 0.001;
    const int pattern_width = 10;
    const int pattern_height = 10;
    const int max_steps = 15000;
    const int max_same = 50;
}

static std::random_device random_device;
static std::mt19937 gen;  // use random_device to seed when you're not testing
static std::normal_distribution<double> weight_distribution(0, sqrt(2.0 / neat_config::num_input));  // TODO: remember to talk about this in the final report
static std::uniform_real_distribution<double> rand_prob(0, 1);
static std::uniform_real_distribution<double> mutate_power(0.5, 1.5);
