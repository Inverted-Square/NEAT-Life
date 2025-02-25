#include "neural_network.hpp"

#include <iostream>

NeuralNetwork::NeuralNetwork(const ints &layers_counts)
{
    if (layers_counts.size() < 2)
    {
        throw std::invalid_argument("NeuralNetwork::NeuralNetwork - You must give enough values for layer_counts so "
                                    "that there's at least an input & output layer.\n");
    }

    int curr_node_id = 0;
    for (unsigned int layer = 0; layer < layers_counts.size(); ++layer)
    {
        for (int count = 0; count < layers_counts[layer]; ++count)
        {
            add_id_if(curr_node_id, layer == 0, layer == layers_counts.size() - 1);
            add_node(curr_node_id++);
        }
    }

    int curr_inno_id = 0;
    int node_sum = 0;
    for (unsigned int layer = 0; layer < layers_counts.size() - 1; ++layer)
    {
        connect_next_layer(node_sum, layers_counts[layer], layers_counts[layer + 1], curr_inno_id);
        node_sum += layers_counts[layer];
    }
}

void NeuralNetwork::add_id_if(const int node_id, const bool is_in, const bool is_out)
{
    if (is_in)
    {
        in_ids.push_back(node_id);
    }

    if (is_out)
    {
        out_ids.push_back(node_id);
    }
}

void NeuralNetwork::connect_next_layer(const int node_sum, const int layer_count, const int next_layer_count,
                                       int &curr_inno_id)
{
    for (int i = node_sum; i < node_sum + layer_count; ++i)
    {
        const Node &from_node = node_refs.at(i);
        for (int j = node_sum + layer_count; j < node_sum + layer_count + next_layer_count; ++j)
        {
            const Node &to_node = node_refs.at(j);
            add_connection(from_node, to_node, curr_inno_id++);
        }
    }
}

const NeuralNetwork::Node &NeuralNetwork::get_node(const int id) const
{
    return node_refs.at(id);
}

const std::unordered_map<int, NeuralNetwork::Node> &NeuralNetwork::get_nodes() const
{
    return node_refs;
}

const NeuralNetwork::innovation_to_conn &NeuralNetwork::get_conn_refs() const
{
    return conn_refs;
}

int NeuralNetwork::get_max_innovation() const
{
    return max_inno;
}

const std::vector<int> &NeuralNetwork::get_out_ids() const
{
    return out_ids;
}

const std::vector<NeuralNetwork::Connection *> &NeuralNetwork::to_node_connections_at(int id) const
{
    return connections.at(id);
}

bool NeuralNetwork::is_in_id(int id) const
{
    // TODO: look into using std::any_of() for this method and the one below
    for (const int in_id : in_ids)
    {
        if (in_id == id)
        {
            return true;
        }
    }

    return false;
}

bool NeuralNetwork::is_out_id(int id) const
{
    for (const int out_id : out_ids)
    {
        if (out_id == id)
        {
            return true;
        }
    }

    return false;
}

bool NeuralNetwork::has_conn(int from_id, int to_id) const
{
    for (const Connection *const conn : connections.at(to_id))
    {
        if (conn->from.id == from_id)
        {
            return true;
        }
    }

    return false;
}

void NeuralNetwork::add_node(int id)
{
    add_node(id, weight_distribution(gen));
}

void NeuralNetwork::add_node(const int id, const double bias)
{
    node_refs.try_emplace(id, id, bias);
}

void NeuralNetwork::add_connection(const NeuralNetwork::Node &from, const NeuralNetwork::Node &to, int innovation)
{
    add_connection(from, to, weight_distribution(gen), innovation);
}

void NeuralNetwork::add_connection(const Node &from, const Node &to, const double weight, const int innovation,
                                   const bool enabled)
{
    max_inno = innovation > max_inno ? innovation : max_inno;
    conn_refs.try_emplace(innovation, from, to, weight, innovation, enabled);
    connections[to.id].push_back(&conn_refs.at(innovation));
}

void NeuralNetwork::mutate_weights()
{
    for (auto &[id, node] : node_refs)
    {
        node.bias = rand_prob(gen) < neat_config::weight_prob ? weight_distribution(gen) : node.bias;
    }

    for (auto &node_connections : connections)
    {
        for (auto &conn : node_connections.second)
        {
            conn->weight = rand_prob(gen) < neat_config::weight_prob && conn->enabled ? weight_distribution(gen)
                                                                                      : conn->weight;
            conn->enabled = conn->enabled || rand_prob(gen) < neat_config::weight_prob;  // TODO: maybe use a different probability for this
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
    child.in_ids = in_ids;
    child.out_ids = out_ids;
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
    // TODO: tbh as add_node does try emplace anyways these if statements might not be needed
    const Node &from_node = conn->from;
    if (node_refs.count(from_node.id) == 0)
    {
        add_node(from_node.id, from_node.bias);
    }

    const Node &to_node = conn->to;
    if (node_refs.count(to_node.id) == 0)
    {
        add_node(to_node.id, to_node.bias);
    }

    Node &cf_node = node_refs.at(from_node.id);
    Node &ct_node = node_refs.at(to_node.id);
    add_connection(cf_node, ct_node, conn->weight, conn->innovation, conn->enabled);
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

doubles NeuralNetwork::activate(doubles &activation_values)
{
    if (activation_values.size() != neat_config::num_input)
    {
        std::cout << "You have activated with the wrong number of activation_values.\nExpected: "
                  << neat_config::num_input << ", got: " << activation_values.size() << "\n";
        exit(EXIT_FAILURE);
    }

    reset_nodes();
    scale_values(activation_values);
    for (int i = 0; i < neat_config::num_input; ++i)
    {
        Node &node = node_refs.at(in_ids[i]);
        node.value = activation_values[i];
        node.activated = true;
    }

    doubles outputs(neat_config::num_output);
    int curr_idx = 0;
    for (const int out_id : out_ids)
    {
        activate_node(out_id);
        outputs[curr_idx++] = node_refs.at(out_id).value;
    }

    softmax(outputs);
    return outputs;
}

void NeuralNetwork::reset_nodes()
{
    for (auto &[id, node] : node_refs)
    {
        node.activated = false;
        node.value = 0;
    }
}

void NeuralNetwork::scale_values(doubles &inputs)
{
    double min_value = inputs[0];
    double max_value = inputs[0];
    for (const double input : inputs)
    {
        if (input < min_value)
        {
            min_value = input;
        }
        else if (input > max_value)
        {
            max_value = input;
        }
    }

    if (max_value - min_value == 0)  // usually happens because all values were negative so relu made them all 0
    {
        std::cout << "Note: NeuralNetwork::scale_values couldn't scale the values as they're all the same.\n";
        return;
    }

    for (double &input : inputs)
    {
        input = (input - min_value) / (max_value - min_value);
    }
}

void NeuralNetwork::activate_node(const int id)
{
    Node &node = node_refs.at(id);
    node.activated = true;
    for (const auto &conn : connections.at(id))
    {
        if (!conn->enabled)
        {
            continue;
        }

        if (!conn->from.activated)
        {
            activate_node(id);
        }

        node.value += conn->weight * conn->from.value;
    }

    node.value += node.bias;
    relu(node);
}

void NeuralNetwork::relu(Node &node)
{
    node.value *= 1 - static_cast<double>(node.value < 0);
}

void NeuralNetwork::softmax(doubles &outputs)
{
    double sum = 0;
    for (const double output : outputs)
    {
        sum += exp(output);
    }

    for (double &output : outputs)
    {
        output = exp(output) / sum;
    }
}

std::string NeuralNetwork::to_string() const
{
    std::string network_str = "Neural network:\n";
    for (const auto &[id, node] : node_refs)
    {
        network_str += "Node " + std::to_string(node.id) + " (Bias " + std::to_string(node.bias) + ")\n";
        network_str += node_connections_to_string(node);
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
