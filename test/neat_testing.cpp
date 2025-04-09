#include "neat.hpp"

void set_random_fitnesses(NEAT &neat);

bool set_up_with_zero_population_size()
{
    try
    {
        NEAT neat(0, {1, 1});
    }
    catch (const std::invalid_argument &err)
    {
        std::cout << ": PASSED";
        return true;
    }

    std::cout << ": FAILED - Should have thrown an error for giving a population size of 0";
    return false;
}

bool set_up_with_bad_layer_counts()
{
    try
    {
        NEAT neat(1, {1});
    }
    catch (const std::invalid_argument &err)
    {
        std::cout << ": PASSED";
        return true;
    }

    std::cout << ": FAILED - Should have thrown an error for not giving enough layer counts";
    return false;
}

bool set_up_with_no_errors()
{
    NEAT neat(20, {4, 10, 2});
    if (neat.get_next_innovation_id() != 60 || neat.get_next_node_id() != 16)
    {
        std::cout << ": FAILED - next_innovation_id or next_node_id is set to the wrong value";
        return false;
    }

    const auto &inno_ids = neat.get_innovation_ids();
    const auto &node_ids = neat.get_node_ids();
    if (inno_ids.size() != 60 || !node_ids.empty())
    {
        std::cout << ": FAILED - innovation_ids is set to the wrong value or node_ids is not empty";
        return false;
    }

    if (neat.get_curr_population().size() != 20)
    {
        std::cout << ": FAILED - the wrong number of individuals were created";
        return false;
    }

    std::cout << ": PASSED";
    return true;
}

bool run_next_generation_on_simple_nets_once_with_one_individual()
{
    NEAT neat(1, {4, 2});
    set_random_fitnesses(neat);
    neat.generate_next_population();
    std::cout << ": 'PASSED'";
    return true;
}

bool run_next_generation_on_simple_nets_many_times_with_one_individual()
{
    NEAT neat(1, {4, 2});
    for (int generation = 0; generation < 10; ++generation)
    {
        set_random_fitnesses(neat);
        neat.generate_next_population();
    }

    std::cout << ": 'PASSED'";
    return true;
}

bool run_next_generation_on_complex_nets_once_with_one_individual()
{
    NEAT neat(1, {4, 10, 2});
    set_random_fitnesses(neat);
    neat.generate_next_population();
    std::cout << ": 'PASSED'";
    return true;
}

bool run_next_generation_on_complex_nets_many_times_with_one_individual()
{
    NEAT neat(1, {4, 10, 2});
    for (int generation = 0; generation < 10; ++generation)
    {
        set_random_fitnesses(neat);
        neat.generate_next_population();
    }

    std::cout << ": 'PASSED'";
    return true;
}

bool run_next_generation_on_simple_nets_once_with_many_individuals()
{
    NEAT neat(20, {4, 2});
    set_random_fitnesses(neat);
    neat.generate_next_population();
    std::cout << ": 'PASSED'";
    return true;
}

bool run_next_generation_on_simple_nets_many_times_with_many_individuals()
{
    NEAT neat(20, {4, 2});
    for (int generation = 0; generation < 10; ++generation)
    {
        set_random_fitnesses(neat);
        neat.generate_next_population();
    }

    std::cout << ": 'PASSED'";
    return true;
}

bool run_next_generation_on_complex_nets_once_with_many_individuals()
{
    NEAT neat(20, {4, 10, 2});
    set_random_fitnesses(neat);
    neat.generate_next_population();
    std::cout << ": 'PASSED'";
    return true;
}

bool run_next_generation_on_complex_nets_many_times_with_many_individuals()
{
    NEAT neat(100, {4, 10, 2});
    for (int generation = 0; generation < 50; ++generation)
    {
        set_random_fitnesses(neat);
        neat.generate_next_population();
    }

    std::cout << " 'PASSED'\n";
    return true;
}

int main()
{
    std::cout << "NEAT testing\n";
    std::vector<bool (*)()> tests{set_up_with_zero_population_size, set_up_with_bad_layer_counts,
                                  set_up_with_no_errors, run_next_generation_on_simple_nets_once_with_one_individual,
                                  run_next_generation_on_simple_nets_many_times_with_one_individual,
                                  run_next_generation_on_complex_nets_once_with_one_individual,
                                  run_next_generation_on_complex_nets_many_times_with_one_individual,
                                  run_next_generation_on_simple_nets_once_with_many_individuals,
                                  run_next_generation_on_simple_nets_many_times_with_many_individuals,
                                  run_next_generation_on_complex_nets_once_with_many_individuals,
                                  run_next_generation_on_complex_nets_many_times_with_many_individuals};
    int passed = 0;
    for (int test = 0; test < static_cast<int>(tests.size()); ++test)
    {
        std::cout << "TEST " << test + 1;
        passed += tests[test]();
        std::cout << "\n";
    }

    std::cout << passed << '/' << tests.size() << " Tests passed.\n";

    return 0;
}

void set_random_fitnesses(NEAT &neat)
{
    std::uniform_real_distribution<double> rand_fitness(0, 100);
    for (int net = 0; net < static_cast<int>(neat.get_curr_population().size()); ++net)
    {
        neat.set_individual_fitness(net, rand_fitness(gen));
    }
}
