#include "neat.hpp"

static std::random_device random_device;
static std::mt19937 gen(random_device());
static std::uniform_real_distribution<double> rand_prob(0, 1);

NEAT::NEAT() : next_innovation_id(0), next_node_id(0)
{
    for (int net = 0; net < neat_config::population_size; ++net)
    {
        curr_population.emplace_back(neat_config::layer_counts);
    }

    next_innovation_id = static_cast<int>(curr_population[0].get_conn_refs().size());
    for (const int count : neat_config::layer_counts)
    {
        next_node_id += count;
    }

    for (const auto &conn_ref : curr_population[0].get_conn_refs())
    {
        const NeuralNetwork::Connection *conn = conn_ref.second;
        innovation_ids[{conn->from.id, conn->to.id}] = conn->innovation;
    }
}

const std::vector<NeuralNetwork> &NEAT::get_curr_population() const
{
    return curr_population;
}

void NEAT::set_individual_fitness(const int index, const double fitness)
{
    curr_population[index].fitness = fitness;
}

void NEAT::generate_next_population()
{
    double mean_fitness = 0;
    const species_groups groups = network_speciation(mean_fitness);
    for (const auto &group : groups)
    {
        repopulate_species(group, mean_fitness);
    }

    curr_population = next_population;
    next_population.clear();
    mutate_networks();
}

NEAT::species_groups NEAT::network_speciation(double &population_mean_fitness) const
{
    species_groups groups(1);
    groups[0].push_back(&curr_population[0]);
    for (int pop_idx = 1; pop_idx < static_cast<int>(curr_population.size()); ++pop_idx)
    {
        const int groups_idx = find_species(groups, curr_population[pop_idx]);
        if (groups_idx != -1)
        {
            groups[groups_idx].push_back(&curr_population[pop_idx]);
        }
        else
        {
            groups.push_back({&curr_population[pop_idx]});
        }
    }

    for (const auto &group : groups)
    {
        for (const NeuralNetwork *net : group)
        {
            population_mean_fitness += net->fitness / static_cast<double>(group.size());
        }
    }

    population_mean_fitness /= static_cast<double>(curr_population.size());
    return groups;
}

int NEAT::find_species(const NEAT::species_groups &groups, const NeuralNetwork &network)
{
    for (int groups_idx = 0; groups_idx < static_cast<int>(groups.size()); ++groups_idx)
    {
        const int rand_idx = static_cast<int>(round(rand_prob(gen) * static_cast<double>(groups[groups_idx].size())));
        const NeuralNetwork *rand_net = groups[groups_idx][rand_idx];
        if (network == *rand_net)
        {
            return groups_idx;
        }
    }

    return -1;
}

void NEAT::repopulate_species(const NEAT::species &group, const double population_mean_fitness)
{
    double total_adj_fitness = 0;
    for (auto network : group)
    {
        total_adj_fitness += network->fitness;
    }
    total_adj_fitness /= static_cast<double>(group.size());

    const int rounded_size = get_new_size(total_adj_fitness, population_mean_fitness, static_cast<int>(group.size()));
    for (int i = 0; i < rounded_size; ++i)
    {
        const NeuralNetwork *parent1 = run_tournament(group, 3);
        const NeuralNetwork *parent2 = run_tournament(group, 3);

        NeuralNetwork child = *parent1 * *parent2;
        next_population.push_back(child);
    }
}

int NEAT::get_new_size(const double total_adj_fitness, const double population_mean_fitness, const int prev_size)
{
    const double size = round(total_adj_fitness / population_mean_fitness);
    if (size < prev_size && trunc(size - 0.5) == size - 0.5)
    {
        return floor(size);
    }

    return static_cast<int>(round(size));
}

const NeuralNetwork *NEAT::run_tournament(const NEAT::species &group, const int tournament_size)
{
    int rand_idx = static_cast<int>(round(rand_prob(gen) * static_cast<double>(group.size())));
    const NeuralNetwork *best_rand_net = group[rand_idx];
    for (int i = 1; i < tournament_size; ++i)
    {
        rand_idx = static_cast<int>(round(rand_prob(gen) * static_cast<double>(group.size())));
        if (group[rand_idx]->fitness > best_rand_net->fitness)
        {
            best_rand_net = group[rand_idx];
        }
    }

    return best_rand_net;
}

void NEAT::mutate_networks()
{
    for (auto &network : curr_population)
    {
        mutate_add_node(network);
        mutate_add_connection(network);
        network.mutate_weights();
    }
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
