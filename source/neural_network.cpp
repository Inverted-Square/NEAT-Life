#include "neural_network.hpp"

void NeuralNetwork::add_node(const char type)
{
    // this needs to randomly generate a bias
}

void NeuralNetwork::add_node(const char type, const float bias)
{
    nodes.emplace_back(type, bias);
}

void NeuralNetwork::add_connection(const int in, const int out, int innovation_id, const bool enabled)
{
    // this needs to randomly generate a weight
}

void NeuralNetwork::add_connection(const int in, const int out, const float weight, int innovation_id, const bool enabled)
{
    connections.emplace_back(in, out, weight, innovation_id, enabled);
}
