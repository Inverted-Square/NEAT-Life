#include <SFML/Graphics.hpp>

#include "events.hpp"

int main()
{
    auto window = sf::RenderWindow{{800, 800}, "NEAT-Life"};

    sf::CircleShape shape{50.0f};
    shape.setPosition(100.0f, 100.0f);

    window.clear();
    window.draw(shape);
    window.display();

    while (window.isOpen())
    {
        process_events(window);
    }

    return 0;
}
