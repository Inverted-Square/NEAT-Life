#pragma once

#include "neat-life_header.hpp"
#include "neural_network.hpp"

#include <unordered_map>
#include <vector>

class NEAT
{
    typedef std::pair<const int, const int> conn;  // connection (from node 'first' to node 'second')

    struct hash_pair
    {
        size_t operator()(const conn &pair) const
        {
            size_t hash1 = std::hash<int>{}(pair.first);
            size_t hash2 = std::hash<int>{}(pair.second);

            return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
        }
    };

    std::unordered_map<conn, int, hash_pair> innovation_ids;  // tracks what innovation ids were given to connections
    std::unordered_map<int, int> node_ids;  // used to track what ids have been given for new nodes (inno, node_id)

    std::vector<NeuralNetwork> curr_population;
    std::vector<NeuralNetwork> next_population;

    typedef std::vector<const NeuralNetwork *> species;
    typedef std::vector<species> species_groups;

    int next_innovation_id;
    int next_node_id;

public:
    NEAT();

    const std::vector<NeuralNetwork> &get_curr_population() const;

    void set_individual_fitness(int index, double fitness);

    void generate_next_population();

private:
    species_groups network_speciation(double &population_mean_fitness) const;

    static int find_species(const species_groups &groups, const NeuralNetwork &network);

    void repopulate_species(const species &group, double population_mean_fitness);

    static int get_new_size(double total_adj_fitness, double population_mean_fitness, int prev_size);

    static const NeuralNetwork *run_tournament(const species &group, int tournament_size);

    void mutate_networks();

    void mutate_add_connection(NeuralNetwork &network);

    void check_previous_nodes(const NeuralNetwork::node_layers &layer_nodes, const NeuralNetwork::Node &to_node,
                              NeuralNetwork &network);

    void check_previous_node(const NeuralNetwork::Node &from_node, const NeuralNetwork::Node &to_node,
                             NeuralNetwork &network);

    void mutate_add_node(NeuralNetwork &network);

    int node_id(int innovation);

    int connection_innovation(const std::pair<int, int> &from_to_ids);
};
