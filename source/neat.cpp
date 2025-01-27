#include "neat.hpp"

static std::random_device random_device;
static std::mt19937 gen(random_device());
static std::uniform_real_distribution<double> rand_prob(0, 1);

void NEAT::mutate_networks()
{

}

void NEAT::mutate_add_connection(NeuralNetwork &network)
{
    const auto &nodes = network.get_nodes();
    for (int layer = static_cast<int>(nodes.size()) - 1; layer >= 1; --layer)  // layer >= as we skip the input layer
    {
        for (const auto &node : nodes.at(layer))
        {
            check_previous_nodes(nodes, node, network);
        }
    }
}

void NEAT::check_previous_nodes(const NeuralNetwork::node_layers &layer_nodes, const NeuralNetwork::Node &to_node,
                                NeuralNetwork &network)
{
    for (int layer = to_node.layer - 1; layer >= 0; --layer)
    {
        for (const auto &from_node : layer_nodes.at(layer))
        {
            check_previous_node(from_node, to_node, network);
        }
    }
}

void NEAT::check_previous_node(const NeuralNetwork::Node &from_node, const NeuralNetwork::Node &to_node,
                               NeuralNetwork &network)
{
    const std::pair<int, int> &from_to_ids = {from_node.id, to_node.id};
    if (innovation_ids.count(from_to_ids) != 0)
    {
        const int conn_inno = innovation_ids.at(from_to_ids);
        if (network.get_conn_refs().count(conn_inno) != 0)
        {
            return;
        }

        if (rand_prob(gen) < 0.2)
        {
            network.add_connection(from_node, to_node, conn_inno);
        }

        return;
    }

    if (rand_prob(gen) < 0.2)
    {
        // TODO: maybe just copy the useful part of connection_innovation as some of the assumptions in it are already known to be true here
        const int conn_inno = connection_innovation(from_to_ids);
        network.add_connection(from_node, to_node, conn_inno);
    }
}

void NEAT::mutate_add_node(NeuralNetwork &network)
{
    const NeuralNetwork::innovation_to_conn &conn_refs = network.get_conn_refs();
    for (const auto &inno_conn : conn_refs)
    {
        if (rand_prob(gen) < 0.2)
        {
            network.disable_connection(inno_conn.first);
            const auto conn = inno_conn.second;
            const int new_node_id = node_id(conn->innovation);
            const int layer = network.make_space_between(conn->from.layer, conn->to.layer);
            network.add_node(new_node_id, layer);
            const NeuralNetwork::Node &new_node = network.get_nodes().at(layer).back();
            const int conn1_inno = connection_innovation({conn->from.id, new_node.id});
            const int conn2_inno = connection_innovation({new_node.id, conn->to.id});
            network.add_connection(conn->from, new_node, conn1_inno);
            network.add_connection(new_node, conn->to, conn2_inno);
        }
    }
}

int NEAT::node_id(const int innovation)
{
    if (node_ids.count(innovation) == 0)
    {
        node_ids[innovation] = next_node_id;
        return next_node_id++;
    }

    return node_ids.at(innovation);
}

int NEAT::connection_innovation(const std::pair<int, int> &from_to_ids)
{
    if (innovation_ids.count(from_to_ids) == 0)
    {
        innovation_ids[from_to_ids] = next_innovation_id;
        return next_innovation_id++;
    }

    return innovation_ids.at(from_to_ids);
}
