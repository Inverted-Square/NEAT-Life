#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class EventHandler
{
    bool up_held = false;
    bool down_held = false;
    bool left_held = false;
    bool right_held = false;
    const bool *const keys_held[4] = {&up_held, &down_held, &left_held, &right_held};
    int scroll_direction = 0;
    const float MOVEMENTS[4][2] = {{0,  -1},
                                   {0,  1},
                                   {-1, 0},
                                   {1,  0}};

public:
    void process_events(sf::RenderWindow &window);

    void update_view(sf::RenderWindow &window, sf::View &view) const;
};
