#include "neural_network.hpp"

#include <iostream>
#include <unordered_set>

NeuralNetwork::NeuralNetwork(const ints &layers_counts)
{
    if (layers_counts.size() < 2)
    {
        throw std::invalid_argument("NeuralNetwork::NeuralNetwork - You must give enough values for layer_counts so "
                                    "that there's at least an input & output layer.\n");
    }

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

const NeuralNetwork::node_layers &NeuralNetwork::get_nodes() const
{
    return nodes;
}

const NeuralNetwork::innovation_to_conn &NeuralNetwork::get_conn_refs() const
{
    return conn_refs;
}

void NeuralNetwork::connect_to_next_layer(int layer, int &innovation_id)
{
    for (const auto &from_node : nodes.at(layer))
    {
        for (const auto &to_node : nodes.at(layer + 1))
        {
            add_connection(from_node, to_node, innovation_id++);
        }
    }
}

void NeuralNetwork::add_node(int id, int layer)
{
    add_node(id, weight_distribution(gen), layer);
}

void NeuralNetwork::add_node(const int id, const double bias, const int layer)
{
    if (layer < 0)
    {
        throw std::invalid_argument("NeuralNetwork::add_node - Ensure that the given layer is positive.");
    }

    nodes[layer].emplace_back(id, bias, layer);
}

// TODO: consider making this 2 methods, one to test if there's a layer between and one to make space between 2 layers
//  tbh I don't think it's needed as the code to make space between will be the same bc the same checks need to be made
int NeuralNetwork::make_space_between(const int layer1, const int layer2)
{
    if (layer1 >= layer2 || layer1 < 0 || nodes.count(layer1) == 0 || nodes.count(layer2) == 0)
    {
        throw std::invalid_argument("NeuralNetwork::add_node_between - Ensure that the given layers exist, are "
                                    "positive and that layer1 < layer2.");
    }

    int new_layer = (layer1 + layer2) / 2;
    if (new_layer == layer1)
    {
        for (int layer = static_cast<int>(nodes.size()) - 1; layer >= layer2; --layer)
        {
            auto node_handler = nodes.extract(layer);
            node_handler.key() = layer + 1;
            nodes.insert(std::move(node_handler));
        }

        ++new_layer;
    }

    return new_layer;
}

void NeuralNetwork::add_connection(const NeuralNetwork::Node &from, const NeuralNetwork::Node &to, int innovation)
{
    add_connection(from, to, weight_distribution(gen), innovation);
}

void NeuralNetwork::add_connection(const Node &from, const Node &to, const double weight, const int innovation)
{
    if (innovation <= max_innovation)
    {
        throw std::invalid_argument("NeuralNetwork::add_connection - bad innovation given. Innovation should always be "
                                    "increasing as connections are never removed.");
    }

    max_innovation = innovation;
    connections[to.id].emplace_back(from, to, weight, innovation, true);
    Connection conn = connections.at(to.id).back();
    conn_refs[conn.innovation] = &conn;
}

void NeuralNetwork::mutate_weights()
{
    for (auto &layer_nodes : nodes)
    {
        for (auto &node : layer_nodes.second)
        {
            node.bias = rand_prob(gen) < 0.1 ? weight_distribution(gen) : node.bias;
        }
    }

    for (auto &node_connections : connections)
    {
        for (auto &conn : node_connections.second)
        {
            conn.weight = rand_prob(gen) < 0.1 && conn.enabled ? weight_distribution(gen) : conn.weight;
        }
    }
}

void NeuralNetwork::disable_connection(int inno)
{
    conn_refs.at(inno)->enabled = false;
}

NeuralNetwork NeuralNetwork::operator*(const NeuralNetwork &other_net) const
{
    NeuralNetwork child;
    std::unordered_set<int> added_nodes;
    const bool chosen_parent = rand_prob(gen) < 0.5;
    for (const auto &inno_conn : conn_refs)
    {
        const int inno = inno_conn.first;
        if (other_net.conn_refs.count(inno) != 0)
        {
            child.inherit_either_parent(inno_conn.second, other_net.conn_refs.at(inno), added_nodes);
        }
        else if ((fitness == other_net.fitness && chosen_parent) || fitness > other_net.fitness)
        {
            child.inherit_gene(inno_conn.second, added_nodes);
        }
    }

    for (const auto &other_inno_conn : other_net.conn_refs)
    {
        const int other_inno = other_inno_conn.first;
        if (conn_refs.count(other_inno) == 0 &&
            ((fitness == other_net.fitness && !chosen_parent) || other_net.fitness > fitness))
        {
            child.inherit_gene(other_inno_conn.second, added_nodes);
        }
    }

    return child;
}

void NeuralNetwork::inherit_either_parent(const Connection *const conn1, const Connection *const conn2,
                                          std::unordered_set<int> &added_nodes)
{
    if (rand_prob(gen) < 0.5)
    {
        inherit_gene(conn1, added_nodes);
    }
    else
    {
        inherit_gene(conn2, added_nodes);
    }
}

void NeuralNetwork::inherit_gene(const Connection *const conn, std::unordered_set<int> &added_nodes)
{
    const Node &from_node = conn->from;
    if (added_nodes.count(from_node.id) == 0)
    {
        add_node(from_node.id, from_node.bias, from_node.layer);
        added_nodes.insert(from_node.id);
    }

    const Node &to_node = conn->to;
    if (added_nodes.count(to_node.id) == 0)
    {
        add_node(to_node.id, to_node.bias, to_node.layer);
        added_nodes.insert(to_node.id);
    }

    const Node &child_from_node = find_node_at(from_node, from_node.layer);
    const Node &child_to_node = find_node_at(to_node, to_node.layer);
    add_connection(child_from_node, child_to_node, conn->weight, conn->innovation);
}

const NeuralNetwork::Node &NeuralNetwork::find_node_at(const Node &given_node, const int layer) const
{
    for (const auto &node : nodes.at(layer))
    {
        if (node.id == given_node.id)
        {
            return node;
        }
    }

    // TODO: don't think this is best practice but reaching here shouldn't be possible
    throw std::runtime_error("NeuralNetwork::find_node_at - couldn't find the given node at that layer");
}

bool NeuralNetwork::operator==(const NeuralNetwork &other_net) const
{
    int disjoint_count = 0;
    int excess_count = 0;
    int matching_count = 0;
    double weight_diff_sum = 0;
    for (const auto &inno_conn : conn_refs)
    {
        const int inno = inno_conn.first;
        const Connection *const conn = inno_conn.second;
        if (other_net.conn_refs.count(inno) != 0)
        {
            ++matching_count;
            weight_diff_sum += fabs(conn->weight - other_net.conn_refs.at(inno)->weight);
            continue;
        }

        disjoint_count += inno < other_net.max_innovation;
        excess_count += inno > other_net.max_innovation;
    }

    for (const auto &other_inno_conn : other_net.conn_refs)
    {
        const int other_inno = other_inno_conn.first;
        if (conn_refs.count(other_inno) == 0)
        {
            disjoint_count += other_inno < max_innovation;
            excess_count += other_inno > max_innovation;
        }
    }

    const int net1_count = static_cast<int>(conn_refs.size());
    const int net2_count = static_cast<int>(other_net.conn_refs.size());
    const double max_genes_count = net1_count < net2_count ? net1_count : net2_count;
    const double average_weight_diff = weight_diff_sum / matching_count;

    // c1 = 1, c2 = 1, c3 = 3, ct = 4
    return excess_count / max_genes_count + disjoint_count / max_genes_count + 3 * average_weight_diff < 4;
}

doubles NeuralNetwork::activate(const doubles &activation_values)
{
    if (activation_values.size() != nodes.at(0).size())
    {
        std::cout << "You have activated with the wrong number of activation_values.\nExpected: " << nodes.at(0).size()
                  << ", got: " << activation_values.size() << "\n";
        exit(EXIT_FAILURE);
    }

    // set and scale the inputs to the network
    int curr_idx = 0;
    for (auto &node : nodes.at(0))
    {
        node.value = activation_values[curr_idx++];
    }
    scale_values(0);

    // feed-forward through hidden layers
    const int nodes_size = static_cast<int>(nodes.size());
    for (int i = 1; i < nodes_size - 1; ++i)
    {
        feed_layer(i, 'r');
    }

    // feed-forward through output layer
    feed_layer(nodes_size - 1, 's');

    doubles outputs(nodes.at(nodes_size - 1).size());
    curr_idx = 0;
    for (const auto &node : nodes.at(nodes_size - 1))
    {
        outputs[curr_idx++] = node.value;
    }

    return outputs;
}

void NeuralNetwork::feed_layer(const int layer, const char activation_func)
{
    for (auto &node : nodes.at(layer))
    {
        for (const auto &connection : connections.at(node.id))
        {
            if (!connection.enabled) continue;  // TODO: decide if you are ok with this coding style to avoid nesting
            node.value += connection.from.value * connection.weight;
        }

        node.value += node.bias;
    }

    if (activation_func == 'r')
    {
        scale_values(layer);
        relu(layer);
    }
    else
    {
        softmax();
    }
}

void NeuralNetwork::scale_values(const int layer)  // doubles &values
{
    double min_value = nodes.at(layer)[0].value;
    double max_value = nodes.at(layer)[0].value;
    for (const auto &node : nodes.at(layer))
    {
        if (node.value < min_value)
        {
            min_value = node.value;
        }
        else if (node.value > max_value)
        {
            max_value = node.value;
        }
    }

    if (max_value - min_value == 0)  // usually happens because all values were negative so relu made them all 0
    {
        std::cout << "Note: NeuralNetwork::scale_values couldn't scale the values as they're all the same.\n";
        return;
    }

    for (auto &node : nodes.at(layer))
    {
        node.value = (node.value - min_value) / (max_value - min_value);
    }
}

void NeuralNetwork::relu(const int layer)
{
    for (auto &node : nodes.at(layer))
    {
        node.value *= 1 - static_cast<double>(node.value < 0);
    }
}

void NeuralNetwork::softmax()
{
    const int nodes_size = static_cast<int>(nodes.size());
    double sum = 0;
    for (const auto &node : nodes.at(nodes_size - 1))
    {
        sum += exp(node.value);
    }

    for (auto &node : nodes.at(nodes_size - 1))
    {
        node.value = exp(node.value) / sum;
    }
}

std::string NeuralNetwork::to_string() const
{
    std::string network_str = "Neural network:\n";
    for (const auto &layer : nodes)
    {
        for (const auto &node : layer.second)
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
        connections_str += "    " + std::to_string(connection.from.id) + arrow + std::to_string(node.id) + " (Weight " +
                           std::to_string(connection.weight) + ")\n";
    }

    return connections_str;
}

std::ostream &operator<<(std::ostream &os, NeuralNetwork &neural_network)
{
    return os << neural_network.to_string();
}
