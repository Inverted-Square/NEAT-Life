cmake_minimum_required(VERSION 3.28)
project(NEAT_Life)

set(CMAKE_CXX_STANDARD 17)

add_executable(NEAT-Life source/main.cpp
        source/events.cpp
        headers/events.hpp
        source/board.cpp
        headers/board.hpp
)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR SFML-2.6.1/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics REQUIRED)

include_directories(headers SFML-2.6.1/include)
target_link_libraries(NEAT-Life sfml-system sfml-window sfml-graphics)
