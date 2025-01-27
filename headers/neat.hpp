#pragma once

#include "neural_network.hpp"

#include <unordered_map>
#include <vector>

// TODO: there are major assumptions being made on innovation_ids and node_ids so make sure to set them up correctly
//       when you go on to make the constructor

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

    typedef std::unordered_map<conn, int, hash_pair> conn_id;  // tracks pre-existing connections
    conn_id innovation_ids;  // used to track what innovation ids have been given for new connections
    std::unordered_map<int, int> node_ids;  // used to track what ids have been given for new nodes (inno, node_id)

    typedef std::vector<NeuralNetwork> networks;
    networks population;

    int next_innovation_id = 0;
    int next_node_id = 0;

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
