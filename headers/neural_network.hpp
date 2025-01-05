#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <random>

typedef std::vector<double> doubles;
typedef std::vector<int> ints;

class NeuralNetwork
{
    struct Node
    {
        const int id;
        double bias;

        Node(const int id, const double bias) : id(id), bias(bias)
        {}
    };

    typedef std::vector<Node> node_layer;
    typedef std::vector<node_layer> node_layers;
    node_layers nodes;

    struct Connection
    {
        const int in;
        double weight;
        const int innovation;
        bool enabled;

        Connection(const int in, const double weight, const int innovation, const bool enabled) :
                in(in), weight(weight), innovation(innovation), enabled(enabled)
        {}
    };

    typedef std::unordered_map<int, std::vector<Connection>> to_node_connections;
    to_node_connections connections;

public:
    explicit NeuralNetwork(const ints &layers_counts);

private:
    void connect_to_next_layer(int layer, int &innovation_id, std::normal_distribution<double> &dis);

    void add_node(int id, int layer, double bias);

    void add_connection(int in, int out, double weight, int innovation);

public:
    void add_new_layer(int before_layer);

    doubles activate(doubles &activation_values) const;

private:
    doubles feed_layer(doubles &inputs, const node_layer &layer, void (*activation_func)(doubles &)) const;

    static void scale_values(doubles &values);

    static void relu(doubles &values);

    static void softmax(doubles &values);

public:
    std::string to_string() const;

private:
    std::string node_connections_to_string(const Node &node) const;
};

std::ostream &operator<<(std::ostream &os, NeuralNetwork &neural_network);
