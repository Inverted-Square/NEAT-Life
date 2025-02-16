#pragma once

#include "neat-life_header.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

typedef std::vector<double> doubles;
typedef std::vector<int> ints;

class NeuralNetwork
{
public:
    struct Node
    {
        const int id;
        double bias;
        int layer;
        double value = 0;

        Node(const int id, const double bias, const int layer) : id(id), bias(bias), layer(layer)
        {}
    };

    typedef std::vector<Node *> node_layer;
    typedef std::unordered_map<int, node_layer> node_layers;

private:
    node_layers nodes;

    std::unordered_map<int, Node> node_refs;

    /** TODO:
     *    after you have done the rest of your tests, remember to improve the structure and naming of the code
     *    also consider places where you should use node_refs
     *    acc maybe do this before you add more tests
     */

public:
    struct Connection
    {
        const Node &from;
        const Node &to;
        double weight;
        const int innovation;
        bool enabled;

        Connection(const Node &from, const Node &to, const double weight, const int innovation, const bool enabled) :
                from(from), to(to), weight(weight), innovation(innovation), enabled(enabled)
        {}
    };

    typedef std::unordered_map<int, std::vector<Connection *>> to_node_connections;

private:
    to_node_connections connections;

public:
    typedef std::unordered_map<int, Connection> innovation_to_conn;

private:
    innovation_to_conn conn_refs;

    int max_inno = -1;

public:
    double fitness = 0;

    NeuralNetwork() = default;

    explicit NeuralNetwork(const ints &layers_counts);

    const node_layers &get_nodes() const;

    const innovation_to_conn &get_conn_refs() const;

    int get_max_innovation() const;

private:
    void connect_to_next_layer(int layer, int &innovation_id);

public:
    void add_node(int id, int layer);

    void add_node(int id, double bias, int layer);

    int make_space_between(int layer1, int layer2);

private:
    void increment_nodes_layer_at(int layer);

public:
    void add_connection(const Node &from, const Node &to, int innovation);

    void add_connection(const Node &from, const Node &to, double weight, int innovation);

    void mutate_weights();

    void disable_connection(int inno);

    // Used for the crossover of two neural networks
    NeuralNetwork operator*(const NeuralNetwork &other_net) const;

private:
    void inherit_either_parent(const Connection *conn1, const Connection *conn2, std::unordered_set<int> &added_nodes);

    void inherit_gene(const Connection *conn, std::unordered_set<int> &added_nodes);

    const NeuralNetwork::Node &find_node_at(const Node &given_node, int layer) const;

public:
    // Used to see if two networks are close enough to belong to the same species_groups
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
