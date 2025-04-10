# NEAT-Life experimentation tool

Author: Ashley Leonard - psyal7

---

## Description

This tool applies the NEAT algorithm to Conway's Game of Life, letting you run different experiments 
(on different board types and with different fitness functions) in order to try and find different 
patterns. Then these patterns are saved so that you can run them again individually (possibly on a 
different board).

---

## Installation and Running

This project was done using CLion, with a CMake file was used to compile and link 
the program - named [CMakeLists.txt](CMakeLists.txt). Looking through this file, 
you will see that it requires the SFML graphics library. The version that was used 
can be downloaded [here](https://www.sfml-dev.org/download/sfml/2.6.1/). Note that 
this project was done on windows, with the MinGW compiler to build the provided 
packages, and so the CMakeLists.txt file was set up for that specifically (in 
other words you made need to make some adjustments to that file).

---

## How to use the project

When you are using this project, there are only 3 files you should need to focus on:
1. [neat-life_header.hpp](headers/neat-life_header.hpp)
2. [util.hpp](headers/util.hpp)
3. [main.cpp](source/main.cpp)

File 1 is where you can configure the setting for a NEAT experiment, along with the
size of the board and the size of the window. Rules and some instructions for each 
variable are given as short comments (this only goes as far to avoid errors and
not everything is given a recommended value). Note that a lot of these values 
shouldn't be made too big for obvious speed reasons.

File 2 is where you'll find a gallery of example patterns to test. You can also use
this area to add in your owns patterns in the same way as they have been done in
this file.

File 3 is the main file you'll be running. The only thing to worry about with this 
file is which of the two functions you call (either run_neat_experiment or 
test_pattern). 

run_neat_experiment will simply start running an experiment using 
the settings you gave in file 1. As it runs, it will show you the best pattern for
a given generation, along with some details about the current generation (note 
that it can take some time before it shows you the best pattern as it has to take
the time to score each neural network).

test_pattern works by simply running the pattern you provide it. For this, you
need to give it a pattern to run from the patterns namespace from file 2
(an example of this is already commented out). 

When running on an infinite board (or a torus board with an infinite axis) you 
can pan the screen with the arrow keys and zoom using the mouse wheel.
