#pragma once

#include <vector>

class NeuralNetwork
{
    static const float bias_min;
    static const float bias_max;
    static const float weight_min;
    static const float weight_max;

    typedef std::pair<const char, float> Node;  // a node is a pair of a type and a bias
    std::vector<Node> nodes;

    struct Connection
    {
        const int in;
        const int out;
        float weight;
        const int innovation_id;
        bool enabled;

        Connection(int in, int out, float weight, int innovation_id, bool enabled) :
                in(in), out(out), weight(weight), innovation_id(innovation_id), enabled(enabled)
        {}
    };

    std::vector<Connection> connections;

public:
    NeuralNetwork(const int num_input, const int num_output)
    {

    }

private:
    void add_node(char type);

    void add_node(char type, float bias);

    void add_connection(int in, int out, int innovation_id, bool enabled = true);

    void add_connection(int in, int out, float weight, int innovation_id, bool enabled = true);
};

const float NeuralNetwork::bias_min = -20.0f;
const float NeuralNetwork::bias_max = 20.0f;
const float NeuralNetwork::weight_min = -20.0f;
const float NeuralNetwork::weight_max = 20.0f;
