#include "neat-life_header.hpp"

#include "events.hpp"
#include "board.hpp"

int main()
{
    auto window = sf::RenderWindow{{win_config::width, win_config::height}, "NEAT-Life", win_config::style};
    auto view = sf::View(sf::FloatRect(0, 0, win_config::width, win_config::height));
    window.setFramerateLimit(win_config::frame_rate);

    // TODO:
    //  - Implement infinite boards

    Board board;
    board.set_cell({25, 25});
    board.set_cell({26, 25});
    board.set_cell({27, 25});
    board.set_cell({26, 24});
    board.set_cell({25, 26});
    board.set_cell({25, 28});
    board.set_cell({25, 29});
    board.set_cell({24, 29});

    board.draw(window);

    auto event_handler = EventHandler();
    while (window.isOpen())
    {
        event_handler.process_events(window);
        event_handler.update_view(window, view);
        board.update_generation();
        board.draw(window);
    }

    return 0;
}
