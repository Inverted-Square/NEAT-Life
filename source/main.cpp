#include <SFML/Graphics.hpp>

#include "events.hpp"
#include "board.hpp"

int main()
{
    auto window = sf::RenderWindow{{700, 700}, "NEAT-Life"};
//    window.setFramerateLimit(60);

    // TODO: you need to look into not having cells anymore (the vector of vectors of booleans)
    //   - instead you need to search live_cells for the {row_index, column_index}
    //   - or think of some other smart why ig

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

    while (window.isOpen())
    {
        process_events(window);
        board.update_generation();
        board.draw(window);
    }

    return 0;
}
