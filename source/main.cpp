#include <SFML/Graphics.hpp>

#include "events.hpp"
#include "board.hpp"

int main()
{
    auto window = sf::RenderWindow{{700, 700}, "NEAT-Life"};
    auto view = sf::View(sf::FloatRect(0, 0, 700, 700));
//    window.setFramerateLimit(60);

    // TODO:
    //  - Implement the vertex array way of drawing
    //  - Implement infinite boards

    Board board(350, 350);
    board.set_cell({25, 25}, true);
    board.set_cell({26, 25}, true);
    board.set_cell({27, 25}, true);
    board.set_cell({26, 24}, true);
    board.set_cell({25, 26}, true);
    board.set_cell({25, 28}, true);
    board.set_cell({25, 29}, true);
    board.set_cell({24, 29}, true);

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
