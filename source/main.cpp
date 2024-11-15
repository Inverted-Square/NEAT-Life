#include <SFML/Graphics.hpp>

#include "events.hpp"
#include "board.hpp"

int main()
{
    auto window = sf::RenderWindow{{1000, 1000}, "NEAT-Life"};
//    window.setFramerateLimit(60);

    Board board(500, 500);
    board.set_cell({25, 25}, true);
    board.set_cell({26, 25}, true);
    board.set_cell({27, 25}, true);
    board.set_cell({26, 24}, true);
    board.set_cell({25, 26}, true);
    board.set_cell({25, 28}, true);
    board.set_cell({25, 29}, true);
    board.set_cell({24, 29}, true);

    board.draw(window);

    while (window.isOpen())
    {
        process_events(window);
        board.update_generation();
        board.draw(window);
    }

    return 0;
}
