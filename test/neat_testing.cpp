#include "neat.hpp"

/* TODO: list of methods
 *   NEAT()
 *   const std::vector<NeuralNetwork> &get_curr_population() const - NOT TESTED (UNNECESSARY)
 *   int get_next_innovation_id() const - NOT TESTED (UNNECESSARY)
 *   int get_next_node_id() const - NOT TESTED (UNNECESSARY)
 *   void set_individual_fitness(int index, double fitness) - NOT TESTED (UNNECESSARY)
 *   void generate_next_population()
 *   species_groups network_speciation(double &population_mean_fitness) const - NOT TESTED (PRIVATE)
 *   static int find_species(const species_groups &groups, const NeuralNetwork &network) - NOT TESTED (PRIVATE)
 *   void repopulate_species(const species &group, double population_mean_fitness) - NOT TESTED (PRIVATE)
 *   static int get_new_size(double total_adj_fitness, double population_mean_fitness, int prev_size) - NOT TESTED (PRIVATE)
 *   static const NeuralNetwork *run_tournament(const species &group) - NOT TESTED (PRIVATE)
 *   void mutate_networks() - NOT TESTED (PRIVATE)
 *   void mutate_add_connection(NeuralNetwork &network) - NOT TESTED (PRIVATE)
 *   void check_previous_nodes(const NeuralNetwork::node_layers &layer_nodes, const NeuralNetwork::Node &to_node,
                              NeuralNetwork &network) - NOT TESTED (PRIVATE)
 *   void check_previous_node(const NeuralNetwork::Node &from_node, const NeuralNetwork::Node &to_node,
                             NeuralNetwork &network) - NOT TESTED (PRIVATE)
 *   void mutate_add_node(NeuralNetwork &network) - NOT TESTED (PRIVATE)
 *   int node_id(int innovation) - NOT TESTED (PRIVATE)
 *   int connection_innovation(const std::pair<int, int> &from_to_ids) - NOT TESTED (PRIVATE)
 * */

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

    neat.generate_next_population();
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

    neat.generate_next_population();
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
    NEAT neat(20, {4, 10, 2});
    for (int generation = 0; generation < 10; ++generation)
    {
        set_random_fitnesses(neat);
        neat.generate_next_population();
    }

    neat.generate_next_population();
    std::cout << ": 'PASSED'";
    return true;
}

/** TODO: WHERE YOU LEFT OFF
 *    - ok so we've now gone through and fixed up issues for test4 so now the breakpoint has been moved for test5
 *    - just ran test5 and it breaks lol (probably due to the std::move() stuff...)
 *    - also note that all of those tests are intended to be gone through in the debugger anyways
 */

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
        passed += tests[test]();  // TODO: I don't like this syntax, we need to find C++'s equivalent to Python's enumerate
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
