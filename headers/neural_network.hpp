#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <random>

// TODO: move these to neat-life_header.hpp and have this file include that file, this is because I want main.cpp to
//       see these too
#define NUM_IN 4
#define NUM_OUT 2

typedef std::vector<double> doubles;
typedef std::vector<int> ints;

class NeuralNetwork
{
    struct Node
    {
        const int id;
        double bias;
        const int layer;
        double value = 0;

        Node(const int id, const double bias, const int layer) : id(id), bias(bias), layer(layer)
        {}
    };

    typedef std::vector<Node> node_layer;
    typedef std::unordered_map<int, node_layer> node_layers;
    node_layers nodes;

    struct Connection
    {
        const Node &from;
        const Node &to;  // TODO: may not be needed (apparently crossover needs it so we'll leave it for now)
        double weight;
        const int innovation;
        bool enabled;

        Connection(const Node &from, const Node &to, const double weight, const int innovation, const bool enabled) :
                from(from), to(to), weight(weight), innovation(innovation), enabled(enabled)
        {}
    };

    typedef std::unordered_map<int, std::vector<Connection>> to_node_connections;
    to_node_connections connections;

    typedef std::unordered_map<int, Connection *> innovation_to_conn;
    innovation_to_conn conn_refs;

    int max_innovation = 0;

public:
    double fitness = 0;

    explicit NeuralNetwork(const ints &layers_counts);

    const node_layers &get_nodes() const;

    const to_node_connections &get_connections() const;

private:
    void connect_to_next_layer(int layer, int &innovation_id);

    void add_node(int id, double bias, int layer);

    void add_connection(const Node &from, const Node &to, double weight, int innovation);

public:
    void mutate_weight();

    void disable_connection(int inno);

    // Used for the crossover of two neural networks
    NeuralNetwork operator*(const NeuralNetwork &other_net) const;

private:
    void inherit_either_parent(const Connection *conn1, const Connection *conn2);

    void inherit_gene(const Connection *conn);

public:
    // Used to see if two networks are close enough to belong to the same species
    bool operator==(const NeuralNetwork &other_net) const;

    doubles activate(const doubles &activation_values);

private:
    void feed_layer(int layer, char activation_func);

    void scale_values(int layer);

    void relu(int layer);

    void softmax();

public:
    std::string to_string() const;

private:
    std::string node_connections_to_string(const Node &node) const;
};

std::ostream &operator<<(std::ostream &os, NeuralNetwork &neural_network);
