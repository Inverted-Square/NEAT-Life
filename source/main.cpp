#include "neat-life_header.hpp"

#include "util.hpp"
#include "events.hpp"
#include "board.hpp"
#include "neat.hpp"

#include <fstream>
#include <algorithm>

void run_neat_experiment();

void test_pattern();

void test_neural_networks();

void test_neat_cgol();

void test_fitness_func();

Board::pattern set_board_using_net(NeuralNetwork &network);

int live_neighbours(const Board::coord &pos, const std::unordered_set<Board::coord, Board::hash_pair> &live_in_pattern,
                    bool infinite);

double methuselah_fitness(Board &board);

double complexity_fitness(Board &board);

double complex_methuselah_fitness(Board &board);

bool is_stable(const std::vector<double> &activities, unsigned int &unique_count);

int main()
{
    run_neat_experiment();
//    test_pattern();
    return EXIT_SUCCESS;
}

void run_neat_experiment()
{
    double (*fitness_func)(Board &board);
    switch (neat_config::fitness_function)
    {
    case 'm':
        fitness_func = methuselah_fitness;
        break;
    case 'c':
        fitness_func = complexity_fitness;
        break;
    default:
        fitness_func = complex_methuselah_fitness;
        break;
    }

    NEAT neat(neat_config::population_size, neat_config::layer_counts);

    auto window = sf::RenderWindow{{win_config::width_u, win_config::height_u}, "NEAT-Life", win_config::style};
    window.setFramerateLimit(win_config::frame_rate);

    auto view = sf::View(sf::FloatRect(0.0f, 0.0f, win_config::width_f, win_config::height_f));
    setup_view(view, neat_config::infinite_board, neat_config::pattern_width, neat_config::pattern_height);
    window.setView(view);

    EventHandler event_handler;
    int curr_gen = 0;
    std::map<Board::pattern, double> pattern_fitnesses;
    Board::pattern best_pattern;
    Board::pattern curr_gen_best_pattern;
    double curr_gen_best_fitness;
    bool displaying_board = false;
    Board board(neat_config::infinite_board);
    std::vector<double> activities;
    std::vector<Board::pattern> saved_patterns;
    int best_gen = 0;
    int curr_displaying_step = 0;
    while (window.isOpen())
    {
        event_handler.process_events(window);
        if (displaying_board)
        {
            event_handler.update_view(window, view);
            board.update_generation();
            board.draw(window);
            activities.push_back(board.average_activity());
            unsigned int unique_count = 0;
            displaying_board = !is_stable(activities, unique_count) && curr_displaying_step++ < neat_config::max_steps;
            continue;
        }

        curr_displaying_step = 0;
        activities.clear();
        curr_gen_best_fitness = 0;
        curr_gen_best_pattern.clear();
        if (curr_gen == neat_config::num_gen)
        {
            board.set_pattern(best_pattern);
            board.draw(window);
            displaying_board = true;
            sf::sleep(sf::milliseconds(1000));
            continue;
        }

        double min_fitness = 0xfffffffffffff;
        double average_fitness = 0;
        auto &networks = neat.get_curr_population();
        for (int net = 0; net < static_cast<int>(networks.size()); ++net)
        {
            Board::pattern set_pattern(set_board_using_net(networks[net]));
            if (pattern_fitnesses.count(set_pattern) != 0)
            {
                const double found_fitness = pattern_fitnesses.at(set_pattern);
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

            Board curr_board(set_pattern, neat_config::infinite_board);
            const double found_fitness = fitness_func(curr_board);
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
            best_gen = curr_gen;
        }

        saved_patterns.push_back(curr_gen_best_pattern);
        board.set_pattern(curr_gen_best_pattern);

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
        board.draw(window);
    }

    std::cout << "Saving experiment\n";
    std::string file_name = "../saved_experiments/";
    time_t timestamp;
    time(&timestamp);
    file_name += ctime(&timestamp);
    file_name.pop_back();
    std::replace(file_name.begin(), file_name.end(), ':', '_');
    file_name += ".txt";
    std::ofstream file(file_name);
    file << "pattern_width = " << neat_config::pattern_width << "\npattern_height = " << neat_config::pattern_height
         << "\ntarget_cells = " << neat_config::target_cells << "\npopulation_size = " << neat_config::population_size
         << "\nnum_gen = " << neat_config::num_gen << "\ntournament_size = " << neat_config::tournament_size
         << "\nweight_prob = " << neat_config::weight_prob << "\nnode_prob = " << neat_config::node_prob
         << "\nconn_prob = " << neat_config::conn_prob << "\nmax_steps = " << neat_config::max_steps << "\nmax_same = "
         << neat_config::max_same << "\nfitness_function = " << neat_config::fitness_function << "\ninfinite_board = "
         << neat_config::infinite_board << "\ninfinite_axis = " << neat_config::infinite_axis << '\n';
    file << "rows = " << board_config::rows << "\ncolumns = " << board_config::columns << '\n';

    file << "\nOther notable patterns:\n\n";

    file << "\n------------------------------------------------------------------\n"
         << "------------------------------------------------------------------\nBEST GEN: " << best_gen << "\nFIT: "
         << pattern_fitnesses.at(best_pattern) << "\n\n";
    for (const auto &pattern_row : best_pattern)
    {
        file << pattern_row << '\n';
    }

    int p_gen = 0;
    for (const auto &pattern : saved_patterns)
    {
        file << "\n------------------------------------------------------------------\nGEN: " << p_gen++ << "\nFIT: "
             << pattern_fitnesses.at(pattern) << "\n\n";
        for (const auto &pattern_row : pattern)
        {
            file << pattern_row << '\n';
        }
    }
    file.close();
}

void test_pattern()
{
    auto window = sf::RenderWindow{{win_config::width_u, win_config::height_u}, "NEAT-Life", win_config::style};
    window.setFramerateLimit(win_config::frame_rate);

    Board board(patterns::big_boi, neat_config::infinite_board);
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
        event_handler.update_view(window, view);
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
            Board board(set_board_using_net(networks[net]), neat_config::infinite_board);
            neat.set_individual_fitness(net, methuselah_fitness(board));
        }

        neat.generate_next_population();
    }
}

void test_fitness_func()
{
    Board board(patterns::u, neat_config::infinite_board);
    std::cout << methuselah_fitness(board);
}

Board::pattern set_board_using_net(NeuralNetwork &network)
{
    Board::pattern net_pattern(neat_config::pattern_height);
    std::unordered_set<Board::coord, Board::hash_pair> live_in_pattern;
    int total_live = 0;
    std::vector<double> inputs(neat_config::area + 3);
    for (int row = 0; row < neat_config::pattern_height; ++row)
    {
        for (int column = 0; column < neat_config::pattern_width; ++column)
        {
            inputs[neat_config::area] = row;
            inputs[neat_config::area + 1] = column;
            inputs[neat_config::area + 2] = total_live;
            const std::vector<double> &outputs = network.activate(inputs);
            const bool light_cell =
                    outputs[0] > outputs[1];// && outputs[0] > rand_prob(gen);  // > rand_prob(gen);  // > outputs[1];
            net_pattern[row] += light_cell ? 'O' : '-';
            if (light_cell)
            {
                inputs[row * neat_config::pattern_width + column] = 1;
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

double methuselah_fitness(Board &board)
{
    const double live_cell_scale =
            1 - abs(static_cast<int>(board.get_num_live()) - neat_config::target_cells) / neat_config::max_distance;
    std::vector<double> activities;
    for (int step = 0; step < neat_config::max_steps; ++step)
    {
        board.update_generation();
        activities.push_back(board.average_activity());
        unsigned int unique_count = 0;
        if (is_stable(activities, unique_count))
        {
            const double fitness = step + 1 - neat_config::max_same;
            return (fitness == 0 ? unique_count : fitness) * live_cell_scale;
        }
    }

    return neat_config::max_steps * live_cell_scale;
}

double complexity_fitness(Board &board)
{
    unsigned int total_complexity = 0;
    for (int step = 0; step < 1000; ++step)
    {
        board.update_generation();
        total_complexity += board.get_complexity();
    }

    return total_complexity / 1000.0;
}

double complex_methuselah_fitness(Board &board)
{
    std::vector<double> activities;
    int max_complexity = board.get_complexity();
    for (int step = 0; step < neat_config::max_steps; ++step)
    {
        board.update_generation();
        const int curr_complexity = board.get_complexity();
        if (curr_complexity > max_complexity)
        {
            max_complexity = curr_complexity;
        }

        activities.push_back(board.average_activity());
        unsigned int unique_count = 0;
        if (is_stable(activities, unique_count))
        {
            const double fitness = step + 1 - neat_config::max_same;
            return 1.5 * (fitness == 0 ? unique_count : fitness) + max_complexity;
        }
    }

    return 1.5 * neat_config::max_steps + max_complexity;
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
        if (unique_activities.size() > 24)
        {
            unique_count = unique_activities.size();
            return false;
        }
    }

    unique_count = unique_activities.size();
    return true;
}
