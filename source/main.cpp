#include "neat-life_header.hpp"

#include "util.hpp"
#include "events.hpp"
#include "board.hpp"
#include "neat.hpp"

#include <random>
#include <memory>

void test_cgol();

void test_neural_networks();

void test_neat_cgol();

void test_fitness_func();

Board::pattern set_board_using_net(NeuralNetwork &network, bool is_infinite);

int live_neighbours(const Board::coord &pos, const std::unordered_set<Board::coord, Board::hash_pair> &live_in_pattern,
                    bool infinite);

int methuselah_fitness(Board &board);

bool is_stable(const std::vector<double> &activities, unsigned int &unique_count);

int main()
{
    // TODO: continue to look at the networks' structures

    NEAT neat(neat_config::population_size, {4, 2});

    auto window = sf::RenderWindow{{win_config::width_u, win_config::height_u}, "NEAT-Life", win_config::style};
    window.setFramerateLimit(win_config::frame_rate);

    auto view = sf::View(sf::FloatRect(0.0f, 0.0f, win_config::width_f, win_config::height_f));
    setup_view(view, true, neat_config::pattern_width, neat_config::pattern_height);
    window.setView(view);

    EventHandler event_handler;
    int curr_step = 0;
    int curr_gen = 0;
    std::map<Board::pattern, int> pattern_fitnesses;
    Board::pattern best_pattern;
    Board::pattern curr_gen_best_pattern;
    int curr_gen_best_fitness = 0;
    bool displaying_board = false;
    Board board(true);
    while (window.isOpen())
    {
        event_handler.process_events(window);  // note: you should only do this if the board is infinite
        if (displaying_board)
        {
            event_handler.update_view(window, view);
            board.update_generation();
            board.draw(window);
            displaying_board = curr_step++ != curr_gen_best_fitness;
            continue;
        }

        curr_gen_best_fitness = 0;
        curr_gen_best_pattern.clear();
        if (curr_gen == neat_config::num_gen)
        {
            curr_gen_best_fitness = pattern_fitnesses.at(best_pattern);
            board.set_pattern(best_pattern);
            board.draw(window);
            displaying_board = true;
            curr_step = 0;
            sf::sleep(sf::milliseconds(1000));
            continue;
        }

        int min_fitness = INT32_MAX;
        double average_fitness = 0;
        auto &networks = neat.get_curr_population();
        for (int net = 0; net < static_cast<int>(networks.size()); ++net)
        {
            Board::pattern set_pattern(set_board_using_net(networks[net], true));
            if (pattern_fitnesses.count(set_pattern) != 0)
            {
                const int found_fitness = pattern_fitnesses.at(set_pattern);
                if (found_fitness > curr_gen_best_fitness)
                {
                    curr_gen_best_fitness = found_fitness;
                    curr_gen_best_pattern = set_pattern;
                }

                if (found_fitness < min_fitness)
                {
                    min_fitness = found_fitness;
                }

                neat.set_individual_fitness(net, pattern_fitnesses.at(set_pattern));
                average_fitness += found_fitness;
                continue;
            }

            Board curr_board(set_pattern, true);
            const int found_fitness = methuselah_fitness(curr_board);
            if (found_fitness > curr_gen_best_fitness)
            {
                curr_gen_best_fitness = found_fitness;
                curr_gen_best_pattern = set_pattern;
            }

            if (found_fitness < min_fitness)
            {
                min_fitness = found_fitness;
            }

            pattern_fitnesses.insert({set_pattern, found_fitness});
            neat.set_individual_fitness(net, found_fitness);
            average_fitness += found_fitness;
        }

        if (best_pattern.empty() || curr_gen_best_fitness > pattern_fitnesses.at(best_pattern))
        {
            best_pattern = curr_gen_best_pattern;
        }

        std::cout << "\n------------------------------------------------------------------\n";
        std::cout << "Generation " << curr_gen++ << '\n';
        std::cout << "Worst fitness: " << min_fitness << '\n';
        std::cout << "Best fitness: " << curr_gen_best_fitness << '\n';
        std::cout << "Average fitness: " << average_fitness / neat_config::population_size << '\n';
        std::cout << "Average #nodes per network: " << neat.average_num_nodes() << '\n';
        std::cout << "Average #connections per network: " << neat.average_num_conns() << '\n';
        std::cout << "Number of species: " << neat.num_species << '\n';

        neat.generate_next_population();
        displaying_board = true;
        curr_step = 0;
        board.set_pattern(curr_gen_best_pattern);
        board.draw(window);
    }

    return EXIT_SUCCESS;
}

void test_cgol()
{
    auto window = sf::RenderWindow{{win_config::width_u, win_config::height_u}, "NEAT-Life", win_config::style};
    window.setFramerateLimit(win_config::frame_rate);

    Board board(patterns::big_boi, true);
    auto view = sf::View(sf::FloatRect(0.0f, 0.0f, win_config::width_f, win_config::height_f));
    setup_view(view, board.is_infinite(), static_cast<int>(board.get_pattern_width()),
               static_cast<int>(board.get_pattern_height()));
    window.setView(view);
    board.draw(window);

    int steps = 0;
    EventHandler event_handler;
    while (window.isOpen())
    {
        event_handler.process_events(window);
        if (board.is_infinite())
        {
            event_handler.update_view(window, view);
        }

        board.update_generation();
        if (++steps == 736692)  // number of steps for big boi to finish
        {
            std::cout << "Finished\n";
            window.close();
        }

        if (steps % 1000 == 0)
        {
            std::cout << "Ran for " << steps << " steps." << "\n";
        }

        board.draw(window);
    }
}

void test_neural_networks()
{
    NeuralNetwork neural_network(std::vector{neat_config::num_input, neat_config::num_output});
    std::cout << neural_network << "\n";

    std::uniform_real_distribution<double> possible_inputs(0, 9);
    for (int repeats = 0; repeats < 250; ++repeats)
    {
        doubles inputs(neat_config::num_input);
        for (int input_idx = 0; input_idx < neat_config::num_input; ++input_idx)
        {
            inputs[input_idx] = possible_inputs(gen);
            std::cout << inputs[input_idx] << ' ';
        }
        std::cout << '\n';

        for (const double &output : neural_network.activate(inputs))
        {
            std::cout << output << "\n";
        }
        std::cout << "\n";
    }
}

void test_neat_cgol()
{
    NEAT neat(25, {4, 2});
    for (int generation = 0; generation < 100; ++generation)
    {
        auto &networks = neat.get_curr_population();
        for (int net = 0; net < static_cast<int>(networks.size()); ++net)
        {
            Board board(set_board_using_net(networks[net], true), true);
            neat.set_individual_fitness(net, methuselah_fitness(board));
        }

        neat.generate_next_population();
    }

//    Board board(set_board_using_net(neat.get_best_net(), true), true);
}

void test_fitness_func()
{
    Board board(patterns::u, true);
    std::cout << methuselah_fitness(board);
}

Board::pattern set_board_using_net(NeuralNetwork &network, const bool is_infinite)
{
    Board::pattern net_pattern(neat_config::pattern_height);
    std::unordered_set<Board::coord, Board::hash_pair> live_in_pattern;
    int total_live = 0;
    const double area = neat_config::pattern_width * neat_config::pattern_height;
    for (int row = 0; row < neat_config::pattern_height; ++row)
    {
        for (int column = 0; column < neat_config::pattern_width; ++column)
        {
            const int surrounding_live = live_neighbours({row, column}, live_in_pattern, is_infinite);
            const double percent_filled = total_live / area;
            std::vector<double> inputs{static_cast<double>(row), static_cast<double>(column),
                                       static_cast<double>(surrounding_live), percent_filled};
            const std::vector<double> &outputs = network.activate(inputs);
            const bool light_cell = outputs[0] > outputs[1];  // > rand_prob(gen);  // > outputs[1];
            net_pattern[row] += light_cell ? 'O' : '-';
            if (light_cell)
            {
                live_in_pattern.insert({row, column});
                ++total_live;
            }
        }
    }

    return net_pattern;
}

int live_neighbours(const Board::coord &pos, const std::unordered_set<Board::coord, Board::hash_pair> &live_in_pattern,
                    const bool infinite)
{
    int live_count = 0;
    for (const auto &direction : board_config::directions)
    {
        int row_index = pos.first + direction[0];
        int column_index = pos.second + direction[1];
        if (!infinite)
        {
            row_index = Board::neg_mod(row_index, board_config::rows);
            column_index = Board::neg_mod(column_index, board_config::columns);
        }

        if (live_in_pattern.count({row_index, column_index}) != 0)
        {
            ++live_count;
            continue;
        }
    }

    return live_count;
}

int methuselah_fitness(Board &board)
{
    std::vector<double> activities;
    for (int step = 0; step < neat_config::max_steps; ++step)
    {
        board.update_generation();
        activities.push_back(board.average_activity());
        unsigned int unique_count = 0;
        if (is_stable(activities, unique_count))
        {
            const int fitness = step + 1 - neat_config::max_same;
            return static_cast<int>(fitness == 0 ? unique_count : fitness);
        }
    }

    return neat_config::max_steps;
}

bool is_stable(const std::vector<double> &activities, unsigned int &unique_count)
{
    if (activities.size() < neat_config::max_same)
    {
        unique_count = 0;
        return false;
    }

    std::unordered_set<double> unique_activities;
    const int diff = static_cast<int>(activities.size()) - neat_config::max_same;
    const int lower_bound = diff * (1 - (diff < 0));
    for (int i = static_cast<int>(activities.size()) - 1; i >= lower_bound; --i)
    {
        unique_activities.insert(activities[i]);
        if (unique_activities.size() > 12)
        {
            unique_count = unique_activities.size();
            return false;
        }
    }

    unique_count = unique_activities.size();
    return true;
}
