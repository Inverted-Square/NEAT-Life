#pragma once

#include <vector>
#include <string>

class NeuralNetwork
{
    struct Node
    {
        int id;
        char type;
        float bias;

        Node(int id, char type, float bias) : id(id), type(type), bias(bias)
        {}
    };

    std::vector<Node> nodes;

    struct Connection
    {
        const int in;
        const int out;
        float weight;
        const int innovation;
        bool enabled;

        Connection(int in, int out, float weight, int innovation, bool enabled) :
                in(in), out(out), weight(weight), innovation(innovation), enabled(enabled)
        {}
    };

    std::vector<Connection> connections;

public:
    NeuralNetwork(int num_input, int num_output);

    [[nodiscard]] std::string to_string() const;

private:
    void add_node(int id, char type);

    void add_node(int id, char type, float bias);

    void add_connection(int in, int out, int innovation, bool enabled = true);

    void add_connection(int in, int out, float weight, int innovation, bool enabled = true);
};

std::ostream &operator<<(std::ostream &os, NeuralNetwork &nn);
