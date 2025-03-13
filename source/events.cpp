#include "events.hpp"

void EventHandler::process_events(sf::RenderWindow &window)
{
    scroll_direction = 0;
    for (auto event = sf::Event{}; window.pollEvent(event);)
    {
        const bool pressed_esc = event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape;
        if (event.type == sf::Event::Closed || pressed_esc)
        {
            window.close();
            return;
        }

        if (!neat_config::infinite_board && neat_config::infinite_axis == '-')
        {
            continue;
        }

        if (event.type == sf::Event::KeyPressed)
        {
            up_held = event.key.code == sf::Keyboard::Up || up_held;
            down_held = event.key.code == sf::Keyboard::Down || down_held;
            left_held = event.key.code == sf::Keyboard::Left || left_held;
            right_held = event.key.code == sf::Keyboard::Right || right_held;
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            up_held = !(event.key.code == sf::Keyboard::Up && up_held) && up_held;
            down_held = !(event.key.code == sf::Keyboard::Down && down_held) && down_held;
            left_held = !(event.key.code == sf::Keyboard::Left && left_held) && left_held;
            right_held = !(event.key.code == sf::Keyboard::Right && right_held) && right_held;
        }

        if (event.type == sf::Event::MouseWheelMoved)
        {
            scroll_direction = event.mouseWheel.delta;
        }
    }
}

void EventHandler::update_view(sf::RenderWindow &window, sf::View &view) const
{
    int index = 0;
    for (auto key_held : keys_held)
    {
        if (*key_held)
        {
            auto movement = events_config::movements[index];
            view.move(events_config::move_speed * movement[0], events_config::move_speed * movement[1]);
        }

        ++index;
    }

    if (scroll_direction == 1)
    {
        view.zoom(0.9);
    }
    else if (scroll_direction == -1)
    {
        view.zoom(1.1);
    }

    window.setView(view);
}
