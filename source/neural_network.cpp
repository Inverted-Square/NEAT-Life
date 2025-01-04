#include "neural_network.hpp"

#include <iostream>
#include <random>

const double bias_min = -1;
const double bias_max = 1;
const double weight_min = -5;
const double weight_max = 5;

std::random_device random_device;
std::mt19937 gen(random_device());
std::uniform_real_distribution<double> biases_dis(bias_min, bias_max);
std::uniform_real_distribution<double> weights_dis(weight_min, weight_max);

NeuralNetwork::NeuralNetwork(const ints &layers_counts)
{
    if (layers_counts.size() < 2)
    {
        std::cout << "You must give enough values for layer_counts so that there's at least an input & output layer.\n";
        exit(EXIT_FAILURE);
    }

    nodes.resize(layers_counts.size());
    int curr_node_id = 0;
    for (int layer = 0; layer < static_cast<int>(layers_counts.size()); ++layer)
    {
        for (int count = 0; count < layers_counts[layer]; ++count)
        {
            add_node(curr_node_id++, layer);
        }
    }

    int curr_innovation_id = 0;
    for (int layer = 0; layer < static_cast<int>(layers_counts.size()) - 1; ++layer)
    {
        connect_to_next_layer(layer, curr_innovation_id);
    }
}

void NeuralNetwork::connect_to_next_layer(int layer, int &innovation_id)
{
    for (const auto &from_node : nodes[layer])
    {
        for (const auto &to_node : nodes[layer + 1])
        {
            add_connection(from_node.id, to_node.id, innovation_id++);
        }
    }
}

void NeuralNetwork::add_node(const int id, const int layer)
{
    const double bias = biases_dis(gen);
    nodes[layer].emplace_back(id, bias);
}

void NeuralNetwork::add_node(const int id, const int layer, const double bias)
{
    nodes[layer].emplace_back(id, bias);
}

void NeuralNetwork::add_connection(const int in, const int out, int innovation)
{
    const double weight = weights_dis(gen);
    connections[out].emplace_back(in, weight, innovation, true);
}

void NeuralNetwork::add_connection(const int in, const int out, const double weight, int innovation, const bool enabled)
{
    connections[out].emplace_back(in, weight, innovation, enabled);
}

void NeuralNetwork::add_new_layer(int before_layer)
{
    nodes.insert(nodes.begin() + before_layer, std::vector<Node>{});
}

doubles NeuralNetwork::activate(doubles &activation_values) const
{
    if (activation_values.size() != nodes[0].size())
    {
        std::cout << "You have activated with the wrong number of activation_values.\nExpected: " << nodes[0].size()
                  << ", got: " << activation_values.size() << std::endl;
        exit(EXIT_FAILURE);
    }

    // feed-forward through hidden layers
    for (auto i = 1ULL; i < nodes.size() - 1; ++i)
    {
        activation_values = feed_layer(activation_values, nodes[i], relu);
    }

    // feed-forward through output layer
    return feed_layer(activation_values, nodes[nodes.size() - 1], softmax);
}

doubles NeuralNetwork::feed_layer(const doubles &inputs, const node_layer &layer, void (*activation_func)(doubles &)) const
{
    // TODO: scale inputs here

    doubles new_activations(layer.size());
    int curr_node = 0;
    for (const auto &node : layer)
    {
        double weighted_sum = 0.0f;
        int curr_conn = 0;
        for (const auto &connection : connections.at(node.id))
        {
            weighted_sum += inputs[curr_conn++] * connection.weight;
        }

        weighted_sum += node.bias;
        new_activations[curr_node++] = weighted_sum;
    }

    activation_func(new_activations);

    return new_activations;
}

void NeuralNetwork::relu(doubles &values)
{
    for (double &value : values)
    {
        value *= 1 - static_cast<double>(value < 0);
    }
}

void NeuralNetwork::softmax(doubles &values)
{
    double sum = 0.0f;
    for (const double &value : values)
    {
        sum += exp(value);
    }

    for (double &value : values)
    {
        value = exp(value) / sum;
    }
}

std::string NeuralNetwork::to_string() const
{
    std::string network_str = "Neural network:\n";
    for (const auto &layer : nodes)
    {
        for (const auto &node : layer)
        {
            network_str += "Node " + std::to_string(node.id) + " (Bias " + std::to_string(node.bias) + ")\n";
            network_str += node_connections_to_string(node);
        }
    }

    return network_str;
}

std::string NeuralNetwork::node_connections_to_string(const Node &node) const
{
    if (connections.count(node.id) == 0)
    {
        return "";
    }

    std::string connections_str = "  Connections:\n";
    for (const auto &connection : connections.at(node.id))
    {
        connections_str += "    " + std::to_string(connection.in) + "->" + std::to_string(node.id) + " (Weight " +
                           std::to_string(connection.weight) + ")\n";
    }

    return connections_str;
}

std::ostream &operator<<(std::ostream &os, NeuralNetwork &neural_network)
{
    return os << neural_network.to_string();
}
