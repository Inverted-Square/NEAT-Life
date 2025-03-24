#pragma once

#include "board.hpp"

namespace patterns
{
    [[maybe_unused]] const Board::pattern u{
            "OOO",
            "O-O",
            "O-O"
    };

    [[maybe_unused]] const Board::pattern bunnies_10b{
            "O--------",
            "-O-------",
            "--O------",
            "---O-----",
            "--OO-O---",
            "------OOO"};

    [[maybe_unused]] const Board::pattern lidka{
            "------O--",
            "------OOO",
            "---------",
            "---OO---O",
            "---O----O",
            "OOO-----O"};

    [[maybe_unused]] const Board::pattern _101{
        "----OO------OO----",
        "---O-O------O-O---",
        "---O----------O---",
        "OO-O----------O-OO",
        "OO-O-O--OO--O-O-OO",
        "---O-O-O--O-O-O---",
        "---O-O-O--O-O-O---",
        "OO-O-O--OO--O-O-OO",
        "OO-O----------O-OO",
        "---O----------O---",
        "---O-O------O-O---",
        "----OO------OO----"
    };

    [[maybe_unused]] const Board::pattern big_boi{
            "-------------------------OOOO---",
            "-----------------------------O--",
            "-----------------------------O--",
            "------------------------------OO",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "--------------------------------",
            "OOO-----------------------------",
            "----O---------------------------",
            "---O----------------------------",
            "--O-----------------------------",
            "---O----------------------------",
            "----O---------------------------"};

    [[maybe_unused]] const Board::pattern oh_yeah{
            "-------------------------",
            "OOOOOOOOOO---------------",
            "OOOOO-OOOO-------O-------",
            "OO-OOOOOO---------O------",
            "OOOOOOOO----------O------",
            "OOOOOOOO----------O------",
            "OOOOOOOOO----------------",
            "OOOOOOOO-----------O-----",
            "OOOOOOOO------------O----",
            "OOOOOOOO------------O----",
            "OOOOOOOO-------------OO--",
            "OOOOOOO--------------O---",
            "OOOOOOOO-------------O---",
            "OOOOOOOO--------------O--",
            "OOOOOOOO--------------O--",
            "OOOOOOOO---------------O-",
            "OOOOOOOO---------------O-",
            "OOOOOOO-O--------------O-",
            "OOOOOOO-O---------------O",
            "OOOOOOO--O--------------O",
            "OOOOOOO--O---------------",
            "OOOOOOOO-O---------------",
            "OOOOOOOO--O--------------",
            "OOOOOOOO--O--------------",
            "OOOOOOOO---O-------------"
    };

    // cool found patterns below ---------------------------------------------------------------------------------------

    [[maybe_unused]] const Board::pattern tall_demo1{
            "----------------------------------------------------------------------------------------------------",
            "-O--OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO",
            "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO"
    };

    [[maybe_unused]] const Board::pattern tall_demo2{
        "----------------------------------------------------------------------------------------------------",
        "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO",
        "----------------------------------------------------------------------------OOOO-OOOOOOOOOOOOOOOOOOO"
    };

    [[maybe_unused]] const Board::pattern neat_bunnies{
        "-OOO-O--",
        "--O--OO-",
        "--OO---O",
        "-OOOO--O",
        "-O-OOOO-",
        "-O-OO--O",
        "----O---"
    };

    [[maybe_unused]] const Board::pattern neat_shorter_bunnies{
        "------------",
        "-O----------",
        "O-O---------",
        "O--OO-------",
        "-O----------",
        "O-OOO-O-----",
        "O--OOO-OO---",
        "---O--O-OO--",
        "O------O-OO-"
    };

    [[maybe_unused]] const Board::pattern many_live_big{
        "-------------------------",
        "OO-O-O-OOOO-OOO----------",
        "OO--OOO------OOO--OOOO---",
        "O---OO-OOOOOOO-O--O-O----",
        "-O--OO-OOOO-O-OO-OOOOOOOO",
        "-O--OOOO-O-----OOO-OOOOO-",
        "O-OO--O-OOO-O-OO-OO-OO-OO",
        "OOOO-O-OOOO--O-O--OO-O--O",
        "-OOOOOOOOO-OOOO---OOOOOOO",
        "O-O--O-OO--OO-O-O--OO--OO",
        "OO-OO-O--OO--O-O-O----OO-",
        "-------O-OOOOO----O--OO-O",
        "O--O-O--O---OO-O---O----O",
        "OO-OOOO-OO-O-OO-OOO----O-",
        "--O---O----OO----OOO-O-O-",
        "-------O-O--OO-O-O-OO-O-O",
        "OOOO--OOOOOOO-O-OO---OOOO",
        "OO-----OO------OO--OO-OOO",
        "--OO-O--OOOO--OO-OOO--O--",
        "-O--O--OOO-OOOO-O---OOOOO",
        "-OO--OOOOO-O--O--OOO--O-O",
        "OOO---OO-OOO-OO-O-OOOO---",
        "OOO--O-OO-O------O--O-O-O",
        "-OOO--OO--O-O-OOO-OOOOO--",
        "-O--OOO-OOO-OOOOO-O-OO--O"
    };

    [[maybe_unused]] const Board::pattern many_live_small{
        "OOOOOOOOOO",
        "OOOOOOOOOO",
        "-OOOOOOOOO",
        "OOOOOOOOOO",
        "-OOOOOOOOO",
        "-OOO---OOO",
        "O-OOOOOOOO",
        "OOOOOOOOOO",
        "O-OOOOOOOO",
        "--OOOOOOOO"
    };

    [[maybe_unused]] const Board::pattern many_live_big_tube{
        "--OOOO-O-OO-OO-O--OO-----",
        "O-OOOOOOOO-OOOO-O--O-OO--",
        "OOO-O-OOOO--O----OO-OOOO-",
        "OO-OOO-OOOOOO--O-OOOOOOO-",
        "O-O---OOO-OO-OOO----OOO--",
        "---O-OOO--O--OO--O-O-OOO-",
        "OOO-O---O-OO--O------O-OO",
        "O---O--O---OO--OOO---OOOO",
        "O--O-O----OO--OO-O--O---O",
        "O---OOO-O-OO-O-OOO--OOOOO",
        "-O--OOO--O-OOO-O---OOOO-O",
        "-OOO-OO--OOOOOOOO-OO-O--O",
        "-O-OO---O-OOOOO--OO-O--O-",
        "----OO------O--OOO--OOOO-",
        "-O-O--O----OO-O-OO-OO-O-O",
        "-OO--OOO----OO--OOOO--O-O",
        "---OO-O-O--OO--OOOO--OOOO",
        "-------O-OOO-O-----OO----",
        "OO---O-OO-OO-OOOO---OO--O",
        "----O----O---OOO-O--O--OO",
        "OOOOO------OO--OOO-OOOO-O",
        "-OOO-OO-O--OO-OOOOOO-OOO-",
        "OOOOO-OO---OO--OOO-OOO-OO",
        "-O--OO-O-OO-O-O-OO-O-OOO-",
        "---OOO-O-O-------OOOOO-O-"
    };
}

void setup_view(sf::View &view, bool is_infinite, int pattern_width, int pattern_height);

inline float max(float n, float m)
{
    return n > m ? n : m;
}

inline float min(float n, float m)
{
    return n < m ? n : m;
}
