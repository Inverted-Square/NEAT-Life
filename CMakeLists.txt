cmake_minimum_required(VERSION 3.28)
project(NEAT_Life)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Ofast")

add_executable(NEAT-Life source/main.cpp
        source/events.cpp
        headers/events.hpp
        source/board.cpp
        headers/board.hpp
        headers/neat-life_header.hpp
        source/util.cpp
        headers/util.hpp
        source/neural_network.cpp
        headers/neural_network.hpp
        source/neat.cpp
        headers/neat.hpp
)

add_executable(ANN-Test test/neural_net_testing.cpp
        source/events.cpp
        headers/events.hpp
        source/board.cpp
        headers/board.hpp
        headers/neat-life_header.hpp
        source/util.cpp
        headers/util.hpp
        source/neural_network.cpp
        headers/neural_network.hpp
        source/neat.cpp
        headers/neat.hpp
)

add_executable(NEAT-Test test/neat_testing.cpp
        source/events.cpp
        headers/events.hpp
        source/board.cpp
        headers/board.hpp
        headers/neat-life_header.hpp
        source/util.cpp
        headers/util.hpp
        source/neural_network.cpp
        headers/neural_network.hpp
        source/neat.cpp
        headers/neat.hpp
)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR SFML-2.6.1/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics REQUIRED)

include_directories(headers SFML-2.6.1/include)
target_link_libraries(NEAT-Life sfml-system sfml-window sfml-graphics)
target_link_libraries(ANN-Test sfml-system sfml-window sfml-graphics)
target_link_libraries(NEAT-Test sfml-system sfml-window sfml-graphics)
