#include "neural_network.hpp"

#include <iostream>
#include <random>

const float bias_min = -20.0f;
const float bias_max = 20.0f;
const float weight_min = -20.0f;
const float weight_max = 20.0f;

std::random_device random_device;
std::mt19937 gen(random_device());
std::uniform_real_distribution<float> biases_dis(bias_min, bias_max);
std::uniform_real_distribution<float> weights_dis(weight_min, weight_max);

NeuralNetwork::NeuralNetwork(const int num_input, const int num_output)
{
    for (int id = 0; id < num_input; ++id)
    {
        add_node(id, 'i');
    }

    for (int id = num_input; id < num_input + num_output; ++id)
    {
        add_node(id, 'o');
    }

    int innovation_id = 0;
    for (int in = 0; in < num_input; ++in)
    {
        for (int out = num_input; out < num_input + num_output; ++out)
        {
            add_connection(in, out, innovation_id++);
        }
    }
}

void NeuralNetwork::add_node(const int id, const char type)
{
    const float bias = biases_dis(gen);
    nodes.emplace_back(id, type, bias);
}

void NeuralNetwork::add_node(const int id, const char type, const float bias)
{
    nodes.emplace_back(id, type, bias);
}

void NeuralNetwork::add_connection(const int in, const int out, int innovation, const bool enabled)
{
    const float weight = weights_dis(gen);
    connections.emplace_back(in, out, weight, innovation, enabled);
}

void NeuralNetwork::add_connection(const int in, const int out, const float weight, int innovation, const bool enabled)
{
    connections.emplace_back(in, out, weight, innovation, enabled);
}

std::string NeuralNetwork::to_string() const
{
    std::string in_nodes = "Input nodes:";
    std::string hid_nodes = "Hidden nodes:";
    std::string out_nodes = "Output nodes";
    for (const auto &node : nodes)
    {
        switch (node.type)
        {
        case 'i':
            in_nodes += " (" + std::to_string(node.id) + " Bias: " + std::to_string(node.bias) + ")";
            break;
        case 'h':
            hid_nodes += " (" + std::to_string(node.id) + " Bias: " + std::to_string(node.bias) + ")";
            break;
        case 'o':
            out_nodes += " (" + std::to_string(node.id) + " Bias: " + std::to_string(node.bias) + ")";
            break;
        }
    }

    std::string connection_str = "Connections:";
    for (const auto &connection : connections)
    {
        connection_str += " (" + std::to_string(connection.in) + "->" + std::to_string(connection.out) + " Weight: " +
                          std::to_string(connection.weight) + ")";
    }

    return in_nodes + "\n" + hid_nodes + "\n" + out_nodes + "\n" + connection_str;
}

std::ostream &operator<<(std::ostream &os, NeuralNetwork &nn)
{
    return (os << nn.to_string());
}
