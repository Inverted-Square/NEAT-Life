#include "neural_network.hpp"

#include <iostream>
#include <algorithm>

std::random_device random_device;
std::mt19937 gen(random_device());

NeuralNetwork::NeuralNetwork(const ints &layers_counts)
{
    if (layers_counts.size() < 2)
    {
        std::cout << "You must give enough values for layer_counts so that there's at least an input & output layer.\n";
        exit(EXIT_FAILURE);
    }

    nodes.resize(layers_counts.size());
    std::normal_distribution<double> weight_distribution(0, sqrt(2.0 / 4));

    int curr_node_id = 0;
    for (int layer = 0; layer < static_cast<int>(layers_counts.size()); ++layer)
    {
        for (int count = 0; count < layers_counts[layer]; ++count)
        {
            add_node(curr_node_id++, layer, weight_distribution(gen));
        }
    }

    int curr_innovation_id = 0;
    for (int layer = 0; layer < static_cast<int>(layers_counts.size()) - 1; ++layer)
    {
        connect_to_next_layer(layer, curr_innovation_id, weight_distribution);
    }
}

void NeuralNetwork::connect_to_next_layer(int layer, int &innovation_id, std::normal_distribution<double> &dis)
{
    for (const auto &from_node : nodes[layer])
    {
        for (const auto &to_node : nodes[layer + 1])
        {
            add_connection(from_node.id, to_node.id, dis(gen), innovation_id++);
        }
    }
}

void NeuralNetwork::add_node(const int id, const int layer, const double bias)
{
    nodes[layer].emplace_back(id, bias);
}

void NeuralNetwork::add_connection(const int in, const int out, const double weight, int innovation)
{
    connections[out].emplace_back(in, weight, innovation, true);
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
                  << ", got: " << activation_values.size() << "\n";
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

doubles NeuralNetwork::feed_layer(doubles &inputs, const node_layer &layer, void (*activation_func)(doubles &)) const
{
    scale_values(inputs);

    doubles new_activations(layer.size());
    int curr_node = 0;
    for (const auto &node : layer)
    {
        double weighted_sum = 0;
        int curr_conn = 0;
        for (const auto &connection : connections.at(node.id))
        {
            if (!connection.enabled)
            {
                continue;
            }

            weighted_sum += inputs[curr_conn++] * connection.weight;
        }

        weighted_sum += node.bias;
        new_activations[curr_node++] = weighted_sum;
    }

    activation_func(new_activations);

    return new_activations;
}

void NeuralNetwork::scale_values(doubles &values)
{
    double min_value = *std::min_element(values.begin(), values.end());
    double max_value = *std::max_element(values.begin(), values.end());
    if (max_value - min_value == 0)  // usually happens because all values were negative so relu made them all 0
    {
        std::cout << "Note: NeuralNetwork::scale_values couldn't scale the values as they're all the same.\n";
        return;
    }

    for (double &value : values)
    {
        value = (value - min_value) / (max_value - min_value);
    }
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
    double sum = 0;
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
        std::string arrow = connection.enabled ? "->" : "-/>";
        connections_str += "    " + std::to_string(connection.in) + arrow + std::to_string(node.id) + " (Weight " +
                           std::to_string(connection.weight) + ")\n";
    }

    return connections_str;
}

std::ostream &operator<<(std::ostream &os, NeuralNetwork &neural_network)
{
    return os << neural_network.to_string();
}
