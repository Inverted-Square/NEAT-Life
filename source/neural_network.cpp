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
            add_node(curr_node_id++, layer, layer != 0 && layer != static_cast<int>(layers_counts.size() - 1));
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

const NeuralNetwork::Node &NeuralNetwork::get_node(const int id) const
{
    return node_refs.at(id);
}

const NeuralNetwork::innovation_to_conn &NeuralNetwork::get_conn_refs() const
{
    return conn_refs;
}

int NeuralNetwork::get_max_innovation() const
{
    return max_inno;
}

void NeuralNetwork::connect_to_next_layer(int layer, int &innovation_id)
{
    for (const auto &from_node : nodes.at(layer))
    {
        for (const auto &to_node : nodes.at(layer + 1))
        {
            add_connection(*from_node, *to_node, innovation_id++);
        }
    }
}

void NeuralNetwork::add_node(int id, int layer, const bool hidden)
{
    add_node(id, weight_distribution(gen), layer, hidden);
}

void NeuralNetwork::add_node(const int id, const double bias, const int layer, const bool hidden)
{
    if (layer < 0)
    {
        throw std::invalid_argument("NeuralNetwork::add_node - Ensure that the given layer is positive.");
    }

    node_refs.try_emplace(id, id, bias, layer, hidden);
    nodes[layer].push_back(&node_refs.at(id));
}

// TODO: consider making this 2 methods, one to test if there's a layer between and one to make space between 2 layers
//  tbh I don't think it's needed as the code to make space between will be the same bc the same checks need to be made
int NeuralNetwork::make_space_between(const int layer1, const int layer2)
{
    if (layer1 >= layer2 || layer1 < 0 || nodes.count(layer1) == 0 || nodes.count(layer2) == 0)
    {
        throw std::invalid_argument("NeuralNetwork::make_space_between - Ensure that the given layers exist, are "
                                    "positive and that layer1 < layer2.");
    }

    int new_layer = (layer1 + layer2) / 2;
    if (new_layer == layer1)
    {
        for (int layer = static_cast<int>(nodes.size()) - 1; layer >= layer2; --layer)
        {
            increment_nodes_layer_at(layer);
            auto node_handler = nodes.extract(layer);
            node_handler.key() = layer + 1;
            nodes.insert(std::move(node_handler));
        }

        ++new_layer;
    }

    return new_layer;
}

void NeuralNetwork::increment_nodes_layer_at(const int layer)
{
    for (auto &node : nodes.at(layer))
    {
        ++(node->layer);
    }
}

void NeuralNetwork::add_connection(const NeuralNetwork::Node &from, const NeuralNetwork::Node &to, int innovation)
{
    add_connection(from, to, weight_distribution(gen), innovation);
}

void NeuralNetwork::add_connection(const Node &from, const Node &to, const double weight, const int innovation,
                                   const bool enabled)
{
    if (from.layer >= to.layer)
    {
        std::cout << "uh oh\n";
    }

    max_inno = innovation > max_inno ? innovation : max_inno;
    conn_refs.try_emplace(innovation, from, to, weight, innovation, enabled);
    connections[to.id].push_back(&conn_refs.at(innovation));
}

void NeuralNetwork::mutate_weights()
{
    for (auto &layer_nodes : nodes)
    {
        for (auto &node : layer_nodes.second)
        {
            node->bias = rand_prob(gen) < neat_config::weight_prob ? weight_distribution(gen) : node->bias;
        }
    }

    for (auto &node_connections : connections)
    {
        for (auto &conn : node_connections.second)
        {
            conn->weight = rand_prob(gen) < neat_config::weight_prob && conn->enabled ? weight_distribution(gen)
                                                                                      : conn->weight;
        }
    }
}

void NeuralNetwork::disable_connection(int inno)
{
    conn_refs.at(inno).enabled = false;
}

NeuralNetwork NeuralNetwork::operator*(const NeuralNetwork &other_net) const
{
    NeuralNetwork child;
    const bool chosen_parent = rand_prob(gen) < 0.5;
    for (const auto &inno_conn : conn_refs)
    {
        const int inno = inno_conn.first;
        if (other_net.conn_refs.count(inno) != 0)
        {
            child.inherit_either_parent(&inno_conn.second, &other_net.conn_refs.at(inno));
        }
        else if ((fitness == other_net.fitness && chosen_parent) || fitness > other_net.fitness)
        {
            child.inherit_gene(&inno_conn.second);
        }
    }

    for (const auto &other_inno_conn : other_net.conn_refs)
    {
        const int other_inno = other_inno_conn.first;
        if (conn_refs.count(other_inno) == 0 &&
            ((fitness == other_net.fitness && !chosen_parent) || other_net.fitness > fitness))
        {
            child.inherit_gene(&other_inno_conn.second);
        }
    }

    return child;
}

void NeuralNetwork::inherit_either_parent(const Connection *const conn1, const Connection *const conn2)
{
    if (rand_prob(gen) < 0.5)
    {
        inherit_gene(conn1);
    }
    else
    {
        inherit_gene(conn2);
    }
}

void NeuralNetwork::inherit_gene(const Connection *const conn)
{
    // TODO: go to debug - you'll find that in this function conn is correct but the changes we make here make it wrong

    const Node &from_node = conn->from;
    if (node_refs.count(from_node.id) == 0)
    {
        add_node(from_node.id, from_node.bias, from_node.hidden ? from_node.layer : 0, from_node.hidden);
    }

    const Node &to_node = conn->to;
    if (node_refs.count(to_node.id) == 0)
    {
        int layer = to_node.layer;
        if (!to_node.hidden && nodes.size() > 1)
        {
            layer = static_cast<int>(nodes.size() - 1);
        }

        add_node(to_node.id, to_node.bias, layer, to_node.hidden);
    }

    Node &cf_node = node_refs.at(from_node.id);
    Node &ct_node = node_refs.at(to_node.id);
    if (cf_node.layer >= ct_node.layer)
    {
        if (!cf_node.hidden && ct_node.hidden)
        {
            const int out_layer = static_cast<int>(nodes.size() - 1);
            const int new_layer = out_layer == 1 ? make_space_between(0, 1) : 1;
            move_node_to(ct_node.id, new_layer);
            ct_node.layer = new_layer;
        }
        else if (cf_node.hidden && !ct_node.hidden)
        {
            const int out_layer = static_cast<int>(nodes.size() - 1);
            const int new_layer = out_layer - 1 == 0 ? make_space_between(0, 1) : out_layer - 1;
            move_node_to(cf_node.id, new_layer);
            cf_node.layer = new_layer;
        }
        else if (cf_node.hidden && ct_node.hidden)
        {
            const int new_layer = ct_node.layer - 1 == 0 ? make_space_between(0, 1) : ct_node.layer - 1;
            move_node_to(cf_node.id, new_layer);
            cf_node.layer = new_layer;
        }
        else
        {
            // TODO:
            //  - after making the changes on lines 202 and 208 you somehow reach this line
            //  - your current error has output nodes not being in the last layer for some reason
            //  - tbh you could consider removing this else block, idk if it's needed
            //  - before that though, you have to consider what it means to reach here and if that's a problem
            //  - wait it happens when the input node on one network is ahead or at the same place as a output node from a different network
            //  - ok that's odd, defo go into the debugger to see what it's saying

            // TODO: also remember to check notepad if needed for a different and more messy version of this
            throw std::runtime_error("This should be impossible to reach.\n");
        }
    }

    add_connection(cf_node, ct_node, conn->weight, conn->innovation, conn->enabled);
}

void NeuralNetwork::move_node_to(int id, int new_layer)
{
    nodes[new_layer].push_back(&node_refs.at(id));
    const int old_layer = node_refs.at(id).layer;
    std::vector<Node *> &node_layer = nodes.at(old_layer);
    if (node_layer.size() == 1)
    {
        nodes.erase(old_layer);
        return;
    }

    for (unsigned int i = 0; i < node_layer.size(); ++i)
    {
        if (node_layer[i]->id == id)
        {
            node_layer.erase(node_layer.begin() + i);
            return;
        }
    }
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
        const Connection *const conn = &inno_conn.second;
        if (other_net.conn_refs.count(inno) != 0)
        {
            ++matching_count;
            weight_diff_sum += fabs(conn->weight - other_net.conn_refs.at(inno).weight);
            continue;
        }

        disjoint_count += inno < other_net.max_inno;
        excess_count += inno > other_net.max_inno;
    }

    for (const auto &other_inno_conn : other_net.conn_refs)
    {
        const int other_inno = other_inno_conn.first;
        if (conn_refs.count(other_inno) == 0)
        {
            disjoint_count += other_inno < max_inno;
            excess_count += other_inno > max_inno;
        }
    }

    const int net1_count = static_cast<int>(conn_refs.size());
    const int net2_count = static_cast<int>(other_net.conn_refs.size());
    const double max_genes_count = net1_count < net2_count ? net1_count : net2_count;
    const double average_weight_diff = matching_count == 0 ? 0 : weight_diff_sum / matching_count;

    // c1 = 1, c2 = 1, c3 = 3, ct = 4
    return 4 * excess_count / max_genes_count + 4 * disjoint_count / max_genes_count + 3 * average_weight_diff < 3;
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
        node->value = activation_values[curr_idx++];
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
        outputs[curr_idx++] = node->value;
    }

    return outputs;
}

void NeuralNetwork::feed_layer(const int layer, const char activation_func)
{
    for (auto &node : nodes.at(layer))
    {
        for (const auto &connection : connections.at(node->id))
        {
            if (!connection->enabled) continue;  // TODO: decide if you are ok with this coding style to avoid nesting
            node->value += connection->from.value * connection->weight;
        }

        node->value += node->bias;
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
    double min_value = nodes.at(layer)[0]->value;
    double max_value = nodes.at(layer)[0]->value;
    for (const auto &node : nodes.at(layer))
    {
        if (node->value < min_value)
        {
            min_value = node->value;
        }
        else if (node->value > max_value)
        {
            max_value = node->value;
        }
    }

    if (max_value - min_value == 0)  // usually happens because all values were negative so relu made them all 0
    {
        std::cout << "Note: NeuralNetwork::scale_values couldn't scale the values as they're all the same.\n";
        return;
    }

    for (auto &node : nodes.at(layer))
    {
        node->value = (node->value - min_value) / (max_value - min_value);
    }
}

void NeuralNetwork::relu(const int layer)
{
    for (auto &node : nodes.at(layer))
    {
        node->value *= 1 - static_cast<double>(node->value < 0);
    }
}

void NeuralNetwork::softmax()
{
    const int nodes_size = static_cast<int>(nodes.size());
    double sum = 0;
    for (const auto &node : nodes.at(nodes_size - 1))
    {
        sum += exp(node->value);
    }

    for (auto &node : nodes.at(nodes_size - 1))
    {
        node->value = exp(node->value) / sum;
    }
}

std::string NeuralNetwork::to_string() const
{
    std::string network_str = "Neural network:\n";
    for (const auto &layer : nodes)
    {
        for (const auto &node : layer.second)
        {
            network_str += "Node " + std::to_string(node->id) + " (Bias " + std::to_string(node->bias) + ")\n";
            network_str += node_connections_to_string(*node);
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
        std::string arrow = connection->enabled ? "->" : "-/>";
        connections_str +=
                "    " + std::to_string(connection->from.id) + arrow + std::to_string(node.id) + " (Weight " +
                std::to_string(connection->weight) + ")\n";
    }

    return connections_str;
}

std::ostream &operator<<(std::ostream &os, NeuralNetwork &neural_network)
{
    return os << neural_network.to_string();
}
