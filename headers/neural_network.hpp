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
        bool activated = false;
        double value = 0;

        Node(const int id, const double bias) : id(id), bias(bias)
        {}
    };

private:
    std::vector<int> in_ids;

    std::vector<int> out_ids;

    std::unordered_map<int, Node> node_refs;

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

private:
    void add_id_if(int node_id, bool is_in, bool is_out);

    void connect_next_layer(int node_sum, int layer_count, int next_layer_count, int &curr_inno_id);

public:
    const Node &get_node(int id) const;

    const std::unordered_map<int, Node> &get_nodes() const;

    const innovation_to_conn &get_conn_refs() const;

    int get_max_innovation() const;

    bool is_in_id(int id) const;

    bool is_out_id(int id) const;

    bool has_conn(int from_id, int to_id) const;

public:
    void add_node(int id);

    void add_node(int id, double bias);

    void add_connection(const Node &from, const Node &to, int innovation);

    void add_connection(const Node &from, const Node &to, double weight, int innovation, bool enabled = true);

    void mutate_weights();

    void disable_connection(int inno);

    // Used for the crossover of two neural networks
    NeuralNetwork operator*(const NeuralNetwork &other_net) const;

private:
    void inherit_either_parent(const Connection *conn1, const Connection *conn2);

    void inherit_gene(const Connection *conn);

public:
    // Used to see if two networks are close enough to belong to the same species_groups
    bool operator==(const NeuralNetwork &other_net) const;

    doubles activate(doubles &activation_values);

private:
    void reset_nodes();

    static void scale_values(doubles &inputs);

    void activate_node(int id);

    static void relu(Node &node);

    static void softmax(doubles &outputs);

public:
    std::string to_string() const;

private:
    std::string node_connections_to_string(const Node &node) const;
};

std::ostream &operator<<(std::ostream &os, NeuralNetwork &neural_network);
