#include "events.hpp"

void process_events(sf::Window& window)
{
    for (auto event = sf::Event{}; window.pollEvent(event);)
    {
        if (event.type == sf::Event::Closed ||
            event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        {
            window.close();
        }
    }
}
