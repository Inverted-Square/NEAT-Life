#include "neat.hpp"

/* TODO: list of methods
 *   NEAT()
 *   const std::vector<NeuralNetwork> &get_curr_population() const
 *   void set_individual_fitness(int index, double fitness)
 *   void generate_next_population()
 *   species_groups network_speciation(double &population_mean_fitness) const
 *   static int find_species(const species_groups &groups, const NeuralNetwork &network)
 *   void repopulate_species(const species &group, double population_mean_fitness)
 *   static int get_new_size(double total_adj_fitness, double population_mean_fitness, int prev_size)
 *   static const NeuralNetwork *run_tournament(const species &group)
 *   void mutate_networks()
 *   void mutate_add_connection(NeuralNetwork &network)
 *   void check_previous_nodes(const NeuralNetwork::node_layers &layer_nodes, const NeuralNetwork::Node &to_node,
                              NeuralNetwork &network)
 *   void check_previous_node(const NeuralNetwork::Node &from_node, const NeuralNetwork::Node &to_node,
                             NeuralNetwork &network)
 *   void mutate_add_node(NeuralNetwork &network)
 *   int node_id(int innovation)
 *   int connection_innovation(const std::pair<int, int> &from_to_ids)
 * */

int main()
{
    std::cout << "NEAT testing\n";

    return 0;
}
