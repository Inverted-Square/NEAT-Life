#include "neat.hpp"

NEAT::NEAT(const int population_size, const std::vector<int> &layer_counts)
{
    if (population_size < 1 || layer_counts.size() < 2)
    {
        throw std::invalid_argument("NEAT::NEAT - You must have population_size >= 1 and layer_counts must have at "
                                    "least 2 elements.\n");
    }

    for (int net = 0; net < population_size; ++net)
    {
        curr_population.emplace_back(layer_counts);
    }

    next_innovation_id = static_cast<int>(curr_population[0].get_conn_refs().size());
    for (const int count : layer_counts)
    {
        next_node_id += count;
    }

    for (const auto &conn_ref : curr_population[0].get_conn_refs())
    {
        const NeuralNetwork::Connection *conn = &conn_ref.second;
        innovation_ids[{conn->from.id, conn->to.id}] = conn->innovation;
    }
}

const std::vector<NeuralNetwork> &NEAT::get_curr_population() const
{
    return curr_population;
}

int NEAT::get_next_innovation_id() const
{
    return next_innovation_id;
}

int NEAT::get_next_node_id() const
{
    return next_node_id;
}

const std::unordered_map<NEAT::conn, int, NEAT::hash_pair> &NEAT::get_innovation_ids() const
{
    return innovation_ids;
}

const std::unordered_map<int, int> &NEAT::get_node_ids() const
{
    return node_ids;
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

    curr_population = std::move(next_population);
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
        const int rand_idx = static_cast<int>(round(
                rand_prob(gen) * static_cast<double>(groups[groups_idx].size() - 1)));
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
        const NeuralNetwork *parent1 = run_tournament(group);
        const NeuralNetwork *parent2 = run_tournament(group);
        next_population.push_back(*parent1 * *parent2);
    }
}

int NEAT::get_new_size(const double total_adj_fitness, const double population_mean_fitness, const int prev_size)
{
    const double size = total_adj_fitness /
                        population_mean_fitness;  // TODO: this used to have round wrapped around it but I removed it as that seemed dumb (maybe my past self had their reasons though)
    if (size < prev_size && trunc(size - 0.5) == size - 0.5)
    {
        return floor(size);
    }

    return static_cast<int>(round(size));
}

const NeuralNetwork *NEAT::run_tournament(const NEAT::species &group)
{
    int rand_idx = static_cast<int>(round(rand_prob(gen) * static_cast<double>(group.size() - 1)));
    const NeuralNetwork *best_rand_net = group[rand_idx];
    for (int i = 1; i < neat_config::tournament_size; ++i)
    {
        rand_idx = static_cast<int>(round(rand_prob(gen) * static_cast<double>(group.size() - 1)));
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
    std::unordered_map<int, std::unordered_set<int>> reachable_nodes;
    std::unordered_set<int> searched_nodes;
    std::vector<int> node_stack;
    for (const int out_id : network.get_out_ids())
    {
        find_reachable(reachable_nodes, searched_nodes, node_stack, out_id, network);
    }

    for (const auto &[from_id, from_node] : network.get_nodes())
    {
        if (network.is_out_id(from_id))
        {
            continue;
        }

        for (const auto &[to_id, to_node] : network.get_nodes())
        {
            if (from_id == to_id || network.is_in_id(to_id) || network.has_conn(from_id, to_id))
            {
                continue;
            }

            if (rand_prob(gen) < neat_config::conn_prob &&
                (network.is_out_id(to_id) || reachable_nodes.at(to_id).count(from_id) == 0))
            {
                const int inno = connection_innovation({from_id, to_id});
                network.add_connection(network.get_node(from_id), network.get_node(to_id), inno);
            }
        }
    }
}

void NEAT::find_reachable(std::unordered_map<int, std::unordered_set<int>> &reachable_nodes,
                          std::unordered_set<int> &searched_nodes, std::vector<int> &node_stack,
                          const int end_node, const NeuralNetwork &network)
{
    for (const int id : node_stack)
    {
        reachable_nodes[end_node].insert(id);
    }

    if (searched_nodes.count(end_node) != 0)
    {
        return;
    }
    node_stack.push_back(end_node);

    if (!network.is_in_id(end_node))
    {
        for (const auto &conn : network.to_node_connections_at(end_node))
        {
            find_reachable(reachable_nodes, searched_nodes, node_stack, conn->from.id, network);
        }
    }

    searched_nodes.insert(end_node);
    node_stack.pop_back();
}

void NEAT::mutate_add_node(NeuralNetwork &network)
{
    std::vector<int> innos(network.get_conn_refs().size());
    int curr_i = 0;
    for (const auto &[inno, conn] : network.get_conn_refs())
    {
        innos[curr_i++] = inno;
    }

    for (const int inno : innos)
    {
        const auto &conn = network.get_conn_refs().at(inno);
        if (rand_prob(gen) < neat_config::node_prob && conn.enabled)
        {
            network.disable_connection(inno);
            const int new_node_id = node_id(conn.innovation);
//            const int layer = network.make_space_between(conn.from.layer, conn.to.layer);
            network.add_node(new_node_id);
            const NeuralNetwork::Node &new_node = network.get_node(new_node_id);
            const int conn1_inno = connection_innovation({conn.from.id, new_node.id});
            const int conn2_inno = connection_innovation({new_node.id, conn.to.id});
            network.add_connection(conn.from, new_node, conn1_inno);
            network.add_connection(new_node, conn.to, conn2_inno);
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
