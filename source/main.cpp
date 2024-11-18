#include <iostream>

#include "neat-life_header.hpp"

#include "events.hpp"
#include "board.hpp"

int main()
{
    auto window = sf::RenderWindow{{win_config::width_u, win_config::height_u}, "NEAT-Life", win_config::style};
    auto view = sf::View(sf::FloatRect(0, 0, win_config::width_f, win_config::height_f));
    window.setFramerateLimit(win_config::frame_rate);

    Board board;
    board.set_cell({5, 0});
    board.set_cell({5, 1});
    board.set_cell({5, 2});
    board.set_cell({4, 3});
    board.set_cell({3, 3});
    board.set_cell({3, 4});
    board.set_cell({1, 6});
    board.set_cell({0, 6});
    board.set_cell({1, 7});
    board.set_cell({1, 8});
    board.set_cell({3, 8});
    board.set_cell({4, 8});
    board.set_cell({5, 8});

    board.draw(window);

    int steps = 0;
    EventHandler event_handler;
    while (window.isOpen())
    {
        event_handler.process_events(window);
        event_handler.update_view(window, view);
        board.update_generation();
        if (++steps == 29126)
        {
            std::cout << "Finished\n";
            window.close();
        }

        board.draw(window);
    }

    return 0;
}
