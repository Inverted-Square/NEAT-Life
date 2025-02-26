#include "neat-life_header.hpp"

#include "util.hpp"
#include "events.hpp"
#include "board.hpp"
#include "neural_network.hpp"

#include <random>

void test_cgol();

void test_neural_networks();

int main()
{
    test_cgol();
//    test_neural_networks();

    return 0;
}

void test_cgol()
{
    auto window = sf::RenderWindow{{win_config::width_u, win_config::height_u}, "NEAT-Life", win_config::style};
    window.setFramerateLimit(win_config::frame_rate);

    Board board(patterns::big_boi, true);
    auto view = sf::View(sf::FloatRect(0.0f, 0.0f, win_config::width_f, win_config::height_f));
    setup_view(view, board);
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
