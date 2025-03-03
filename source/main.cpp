#include "neat-life_header.hpp"

#include "util.hpp"
#include "events.hpp"
#include "board.hpp"
#include "neat.hpp"

#include <mutex>
#include <thread>
//#include <utility>
#include <future>

std::mutex neat_lock;
std::mutex pattern_fitnesses_lock;
std::mutex curr_best_lock;
std::mutex net_lock;

void score_networks(NEAT &neat, std::map<Board::pattern, int> &pattern_fitnesses, Board::pattern &curr_best, int &net);

int grab_net(int &net);

NeuralNetwork &neat_get_individual(NEAT &neat, int idx);

bool pattern_is_recorded(const std::map<Board::pattern, int> &pattern_fitnesses, const Board::pattern &pattern_set);

int pattern_fitnesses_at(const std::map<Board::pattern, int> &pattern_fitnesses, const Board::pattern &pattern_set);

void pattern_fitnesses_insert(std::map<Board::pattern, int> &pattern_fitnesses, const Board::pattern &pattern_set,
                              int found_fitness);

void neat_set_individual_fitness(NEAT &neat, int index, double fitness);

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
    // TODO: for the multi-threaded stuff, look into std::async
    // TODO: OK, so new todo here... lets try to instead gather all the patterns in a vector (maybe vector of vectors) and don't bother with async as you can't seem
    //       to use references. Then pop off this vector of patterns as you push them with their fitness to to pattern_fitnesses until empty.
    //       Because it seems to be annoying with curr_best, just...tbh idk...ughhhhhhhhhhhhhh
    //         Maybe a map that maps a fitness to a board pattern
    //      OOOOOOOOOOOOOO interesting side idea, if we do get all the patterns before running in parallel we'll be able to preemptively filter them out

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
            displaying_board = curr_step++ != pattern_fitnesses.at(curr_gen_best_pattern);
            continue;
        }

        curr_gen_best_pattern.clear();
        if (curr_gen == neat_config::num_gen)
        {
            curr_gen_best_pattern = best_pattern;
            board.set_pattern(best_pattern);
            board.draw(window);
            displaying_board = true;
            curr_step = 0;
            sf::sleep(sf::milliseconds(1000));
            continue;
        }

        int net = 0;
//        std::vector<std::future<Board::pattern>> futures;
//        for (int i = 0; i < 8; ++i)
//        {
//            futures[i] = std::async(std::launch::async, score_networks, neat, pattern_fitnesses, std::ref(net));
//        }
//
//        for (auto &future : futures)
//        {
//            Board::pattern best_from_thread = future.get();
//            if (pattern_fitnesses.at(best_from_thread) > pattern_fitnesses.at(curr_gen_best_pattern))
//            {
//                curr_gen_best_pattern = best_from_thread;
//            }
//        }

//        Board::pattern q = f.get();

        std::vector<std::thread> threads;
        threads.reserve(1);
        for (int i = 0; i < 1; ++i)
        {
            threads.emplace_back([&]() {score_networks(neat, pattern_fitnesses, curr_gen_best_pattern, net);});
        }

        for (auto &t : threads)
        {
            t.join();
        }

        threads.clear();

        if (pattern_fitnesses.count(curr_gen_best_pattern) == 0)
        {
            std::cout << "err here\n";
        }

        if (pattern_fitnesses.count(best_pattern) == 0 ||
            pattern_fitnesses.at(curr_gen_best_pattern) > pattern_fitnesses.at(best_pattern))
        {
            best_pattern = curr_gen_best_pattern;
        }

        neat.generate_next_population();
        std::cout << "Best fitness of gen " << curr_gen++ << ": " << pattern_fitnesses.at(curr_gen_best_pattern)
                  << '\n';
        displaying_board = true;
        curr_step = 0;
        board.set_pattern(curr_gen_best_pattern);
        board.draw(window);
    }

    return EXIT_SUCCESS;
}

void score_networks(NEAT &neat, std::map<Board::pattern, int> &pattern_fitnesses, Board::pattern &curr_best, int &net)
{
    Board::pattern temp_best;
    for (int curr_net = grab_net(net); curr_net < neat_config::population_size; curr_net = grab_net(net))
    {
        const Board::pattern pattern_set(set_board_using_net(neat_get_individual(neat, curr_net), true));
        if (pattern_is_recorded(pattern_fitnesses, pattern_set))
        {
            neat_set_individual_fitness(neat, curr_net, pattern_fitnesses_at(pattern_fitnesses, pattern_set));
            continue;
        }

        Board curr_board(pattern_set, true);
        const int found_fitness = methuselah_fitness(curr_board);
        pattern_fitnesses_insert(pattern_fitnesses, pattern_set, found_fitness);
        neat_set_individual_fitness(neat, curr_net, found_fitness);
        if (!pattern_is_recorded(pattern_fitnesses, temp_best) ||
            found_fitness > pattern_fitnesses_at(pattern_fitnesses, temp_best))
        {
            temp_best = pattern_set;
        }
    }

    std::lock_guard<std::mutex> lock(curr_best_lock);
    if (curr_best.empty() || pattern_fitnesses_at(pattern_fitnesses, temp_best) > pattern_fitnesses_at(pattern_fitnesses, curr_best))
    {
        curr_best = temp_best;
    }
}

int grab_net(int &net)
{
    std::lock_guard<std::mutex> lock(net_lock);
    return net++;
}

NeuralNetwork &neat_get_individual(NEAT &neat, const int idx)
{
    std::lock_guard<std::mutex> lock(neat_lock);
    return neat.get_individual(idx);
}

bool pattern_is_recorded(const std::map<Board::pattern, int> &pattern_fitnesses, const Board::pattern &pattern_set)
{
    std::lock_guard<std::mutex> lock(pattern_fitnesses_lock);
    return pattern_fitnesses.count(pattern_set) != 0;
}

int pattern_fitnesses_at(const std::map<Board::pattern, int> &pattern_fitnesses, const Board::pattern &pattern_set)
{
    std::lock_guard<std::mutex> lock(pattern_fitnesses_lock);
    return pattern_fitnesses.at(pattern_set);
}

void pattern_fitnesses_insert(std::map<Board::pattern, int> &pattern_fitnesses, const Board::pattern &pattern_set,
                              const int found_fitness)
{
    std::lock_guard<std::mutex> lock(pattern_fitnesses_lock);
    pattern_fitnesses[pattern_set] = found_fitness;
}

void neat_set_individual_fitness(NEAT &neat, const int index, const double fitness)
{
    std::lock_guard<std::mutex> lock(neat_lock);
    neat.set_individual_fitness(index, fitness);
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
            const bool light_cell = outputs[0] > rand_prob(gen);  // > rand_prob(gen);  // > outputs[1];
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
        if (unique_activities.size() > 8)
        {
            unique_count = unique_activities.size();
            return false;
        }
    }

    unique_count = unique_activities.size();
    return true;
}
