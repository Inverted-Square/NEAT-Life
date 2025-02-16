#include <array>
#include "neural_network.hpp"

/* TODO: List of methods
 *   NeuralNetwork() = default - NOT TESTED (UNNECESSARY)
 *   explicit NeuralNetwork(const ints &layers_counts) - HEADERS DONE
 *   const node_layers &get_nodes() const - NOT TESTED (UNNECESSARY)
 *   const innovation_to_conn &get_conn_refs() const - NOT TESTED (UNNECESSARY)
 *   void connect_to_next_layer(int layer, int &innovation_id) - NOT TESTED (PRIVATE)
 *   void add_node(int id, int layer) - HEADERS DONE
 *   void add_node(int id, double bias, int layer) - HEADERS DONE
 *   int make_space_between(int layer1, int layer2) - HEADERS DONE
 *   void add_connection(const Node &from, const Node &to, int innovation) - HEADERS DONE
 *   void add_connection(const Node &from, const Node &to, double weight, int innovation) - HEADERS DONE
 *   void mutate_weights() - HEADERS DONE
 *   void disable_connection(int inno) - NOT TESTED (UNNECESSARY)
 *   NeuralNetwork operator*(const NeuralNetwork &other_net) const - HEADERS DONE
 *   void inherit_either_parent(const Connection *conn1, const Connection *conn2, std::unordered_set<int> &added_nodes) - NOT TESTED (PRIVATE)
 *   void inherit_gene(const Connection *conn, std::unordered_set<int> &added_nodes) - NOT TESTED (PRIVATE)
 *   const NeuralNetwork::Node &find_node_at(const Node &given_node, int layer) const - NOT TESTED (PRIVATE)
 *   bool operator==(const NeuralNetwork &other_net) const - HEADERS DONE
 *   doubles activate(const doubles &activation_values) - HEADERS DONE
 *   void feed_layer(int layer, char activation_func) - NOT TESTED (PRIVATE)
 *   void scale_values(int layer) - NOT TESTED (PRIVATE)
 *   void relu(int layer) - NOT TESTED (PRIVATE)
 *   void softmax() - NOT TESTED (PRIVATE)
 *   std::string to_string() const - NOT TESTED (UNNECESSARY)
 *   std::string node_connections_to_string(const Node &node) const - NOT TESTED (PRIVATE)
 *   std::ostream &operator<<(std::ostream &os, NeuralNetwork &neural_network) - NOT TESTED (UNNECESSARY)
 */

bool check_creation(const std::vector<int> &layer_counts);

bool has_correct_structure(const NeuralNetwork &network, const std::vector<int> &layer_counts);

bool bad_make_space_call(NeuralNetwork &network, int layer1, int layer2);

bool find_layer_make_space_call(NeuralNetwork &network, int layer1, int layer2, int expected_layer);

bool create_neural_net_with_one_layer()
{
    try
    {
        NeuralNetwork network({1});
    }
    catch (const std::invalid_argument &error)
    {
        std::cout << ": PASSED";
        return true;
    }

    std::cout << ": FAILED - a network with 1 layer was created which shouldn't be allowed";
    return false;
}

bool create_neural_net_with_input_and_output_layers()
{
    return check_creation({3, 2});
}

bool create_neural_net_with_one_hidden_layer()
{
    return check_creation({3, 2, 2});
}

bool create_neural_net_with_many_hidden_layers()
{
    return check_creation({3, 4, 5, 4, 2});
}

bool add_node_with_random_bias_and_bad_layer()
{ return true; }

bool add_node_with_random_bias_and_good_layer()
{ return true; }

bool add_node_with_given_bias_and_bad_layer()
{ return true; }

bool add_node_with_given_bias_and_good_layer()
{ return true; }

bool make_space_between_with_layers_being_equal()
{
    NeuralNetwork network({1, 1});
    return bad_make_space_call(network, 0, 0);
}

bool make_space_between_with_layer1_being_greater_than_layer2()
{
    NeuralNetwork network({1, 1});
    return bad_make_space_call(network, 1, 0);
}

bool make_space_between_with_layer1_being_negative()
{
    NeuralNetwork network({1, 1});
    return bad_make_space_call(network, -1, 1);
}

bool make_space_between_with_layer1_not_existing()
{
    NeuralNetwork network({1, 1});
    return bad_make_space_call(network, 2, 3);
}

bool make_space_between_with_layer2_not_existing()
{
    NeuralNetwork network({1, 1});
    return bad_make_space_call(network, 0, 2);
}

bool make_space_between_an_odd_number_of_layers()
{
    NeuralNetwork network({1, 1, 1, 1, 1, 1});
    return find_layer_make_space_call(network, 1, 5, 3);
}

bool make_space_between_an_even_number_of_layers()
{
    NeuralNetwork network({1, 1, 1, 1, 1, 1});
    return find_layer_make_space_call(network, 0, 5, 2);
}

bool make_space_between_with_layer1_and_layer2_being_next_to_each_other()
{
    NeuralNetwork network({2, 3, 4, 5, 6});
    const std::vector<int> expected_layers{0, 1, 3, 4, 5};
    int found_layer = network.make_space_between(1, 2);
    if (found_layer != 2)
    {
        std::cout << ": FAILED - provided wrong layer in-between";
        return false;
    }

    const auto &nodes = network.get_nodes();
    for (const int layer : expected_layers)
    {
        if (nodes.count(layer) == 0)
        {
            std::cout << ": FAILED - inserted new layer incorrectly";
            return false;
        }
    }

    std::cout << ": PASSED";
    return true;
}

bool add_connection_with_random_weight_and_bad_innovation()
{ return true; }

bool add_connection_with_random_weight_and_good_innovation()
{ return true; }

bool add_connection_with_given_weight_and_bad_innovation()
{ return true; }

bool add_connection_with_given_weight_and_good_innovation()
{ return true; }

bool check_mutate_weights_just_changes_the_weights()
{
    NeuralNetwork network1({30, 1});
    NeuralNetwork network2 = network1;
    const auto &refs = network2.get_conn_refs();
    const auto &ns = network2.get_nodes();
    network1.mutate_weights();
    if (!has_correct_structure(network1, {30, 1}))
    {
        return false;
    }

    bool weights_changed = false;
    for (const auto &[inno, conn] : network1.get_conn_refs())
    {
        if (refs.at(inno).weight != conn.weight)
        {
            weights_changed = true;
        }
    }

    for (const auto &[layer, nodes] : network1.get_nodes())
    {
        for (unsigned int i = 0; i < network1.get_nodes().at(layer).size(); ++i)
        {
            if (ns.at(layer)[i]->bias != nodes[i]->bias)
            {
                weights_changed = true;
            }
        }
    }

    if (!weights_changed)
    {
        std::cout << ": FAILED - none of the weights mutated (could be possible that you got unlucky though or the "
                     "probability is low)";
        return false;
    }

    std::cout << ": PASSED";
    return true;
}

bool check_mutate_weights_leaves_disabled_connections()
{
    NeuralNetwork network1({100, 100, 100, 100});
    NeuralNetwork network2 = network1;
    const auto &refs = network2.get_conn_refs();
    for (const auto &[inno, conn] : network1.get_conn_refs())
    {
        network1.disable_connection(inno);
    }

    network1.mutate_weights();
    for (const auto &[inno, conn] : network1.get_conn_refs())
    {
        if (conn.weight != refs.at(inno).weight)
        {
            std::cout << ": FAILED - weights were changed for disabled connections";
            return false;
        }
    }

    std::cout << ": PASSED";
    return true;
}

bool crossover_identical_networks()
{
    NeuralNetwork network1({5, 10, 2});
    NeuralNetwork network2 = network1 * network1;
    const auto &refs = network2.get_conn_refs();
    for (const auto &[inno, conn] : network1.get_conn_refs())
    {
        if (refs.count(inno) == 0)
        {
            std::cout << ": FAILED - Non-matching gene found";
            return false;
        }

        const auto &other_conn = refs.at(inno);
        if (other_conn.enabled != conn.enabled || other_conn.weight != conn.weight || other_conn.to.id != conn.to.id ||
            other_conn.from.id != conn.from.id)
        {
            std::cout << ": FAILED - Matching gene doesn't share same attributes";
            return false;
        }
    }

    if (network1.get_max_innovation() != network2.get_max_innovation())
    {
        std::cout << ": FAILED - wrong max innovation value";
        return false;
    }

    std::cout << ": PASSED";
    return true;
}

bool crossover_networks_with_same_structure()
{
    NeuralNetwork network1({5, 10, 2});
    NeuralNetwork network2({5, 10, 2});
    NeuralNetwork network3 = network1 * network2;
    const auto &parent2_refs = network2.get_conn_refs();
    const auto &child_refs = network3.get_conn_refs();
    for (const auto &[inno, conn] : network1.get_conn_refs())
    {
        if (child_refs.count(inno) == 0)
        {
            std::cout << ": FAILED - Non-matching gene found";
            return false;
        }

        const auto &parent2_conn = parent2_refs.at(inno);
        const auto &child_conn = child_refs.at(inno);
        if (child_conn.enabled != conn.enabled ||
            (child_conn.weight != conn.weight && child_conn.weight != parent2_conn.weight) ||
            child_conn.to.id != conn.to.id || child_conn.from.id != conn.from.id)
        {
            std::cout << ": FAILED - Matching gene doesn't share either parents attributes";
            return false;
        }
    }

    if (network1.get_max_innovation() != network3.get_max_innovation())
    {
        std::cout << ": FAILED - wrong max innovation value";
        return false;
    }

    std::cout << ": PASSED";
    return true;
}

bool crossover_networks_where_network1_is_a_subset_of_the_other()
{
    NeuralNetwork network1({5, 10, 2});
    NeuralNetwork network2({5, 10, 2});
    network2.fitness = network1.fitness + 1;
    for (const int inno : {2, 12, 23, 25, 33, 57})
    {
        network1.disable_connection(inno);
    }

    NeuralNetwork network3 = network1 * network2;
    const auto &parent1_refs = network1.get_conn_refs();
    const auto &child_refs = network3.get_conn_refs();
    for (const auto &[inno, conn] : network2.get_conn_refs())
    {
        if (child_refs.count(inno) == 0)
        {
            std::cout << ": FAILED - Non-matching gene found";
            return false;
        }

        const auto &parent1_conn = parent1_refs.at(inno);
        const auto &child_conn = child_refs.at(inno);
        if (child_conn.enabled != conn.enabled ||
            (child_conn.weight != conn.weight && child_conn.weight != parent1_conn.weight) ||
            child_conn.to.id != conn.to.id || child_conn.from.id != conn.from.id)
        {
            std::cout << ": FAILED - Matching gene doesn't share either parents attributes";
            return false;
        }
    }

    if (network1.get_max_innovation() != network3.get_max_innovation())
    {
        std::cout << ": FAILED - wrong max innovation value";
        return false;
    }

    std::cout << ": PASSED";
    return true;
}

bool crossover_networks_where_network2_is_a_subset_of_the_other()
{
    NeuralNetwork network1({5, 10, 2});
    NeuralNetwork network2({5, 10, 2});
    network2.fitness = network1.fitness + 1;
    for (const int inno : {2, 12, 23, 25, 33, 57})
    {
        network2.disable_connection(inno);
    }

    NeuralNetwork network3 = network1 * network2;
    const auto &parent1_refs = network1.get_conn_refs();
    const auto &child_refs = network3.get_conn_refs();
    for (const auto &[inno, conn] : network2.get_conn_refs())
    {
        if (!conn.enabled)
        {
            continue;
        }

        if (child_refs.count(inno) == 0)
        {
            std::cout << ": FAILED - Non-matching gene found";
            return false;
        }

        const auto &parent1_conn = parent1_refs.at(inno);
        const auto &child_conn = child_refs.at(inno);
        if (child_conn.enabled != conn.enabled ||
            (child_conn.weight != conn.weight && child_conn.weight != parent1_conn.weight) ||
            child_conn.to.id != conn.to.id || child_conn.from.id != conn.from.id)
        {
            std::cout << ": FAILED - Matching gene doesn't share either parents attributes";
            return false;
        }
    }

    if (network1.get_max_innovation() != network3.get_max_innovation())
    {
        std::cout << ": FAILED - wrong max innovation value";
        return false;
    }

    std::cout << ": PASSED";
    return true;
}

bool crossover_networks_with_some_disjoint_and_excess_genes()
{
    NeuralNetwork network1({5, 10, 2});
    NeuralNetwork network2({5, 10, 2});
    for (const int inno : {0, 1, 2, 3, 25, 37})
    {
        network1.disable_connection(inno);
    }

    for (const int inno : {28, 30, 40, 67, 68, 69})
    {
        network2.disable_connection(inno);
    }

    NeuralNetwork network3 = network1 * network2;
    const auto &parent1_refs = network1.get_conn_refs();
    const auto &parent2_refs = network2.get_conn_refs();
    if (parent1_refs.size() + parent2_refs.size() != 2 * (network3.get_conn_refs().size() + 6))
    {
        std::cout << ": FAILED - wrong number of genes in child";
        return false;
    }

    for (const auto &[inno, conn] : network3.get_conn_refs())
    {
        if (parent1_refs.count(inno) != 0 && parent1_refs.at(inno).enabled)
        {
            const auto &p1_conn = parent1_refs.at(inno);
            if (conn.from.id != p1_conn.from.id || conn.to.id != p1_conn.to.id)
            {
                std::cout << ": FAILED - Matching gene doesn't share parent's attributes";
                return false;
            }
        }
        else if (parent2_refs.count(inno) != 0 && parent2_refs.at(inno).enabled)
        {
            const auto &p2_conn = parent2_refs.at(inno);
            if (conn.from.id != p2_conn.from.id || conn.to.id != p2_conn.to.id)
            {
                std::cout << ": FAILED - Matching gene doesn't share parent's attributes";
                return false;
            }
        }
    }

    if (network3.get_max_innovation() != 66 && network3.get_max_innovation() != 69)
    {
        std::cout << ": FAILED - wrong max innovation value";
        return false;
    }

    std::cout << ": PASSED";
    return true;
}

bool crossover_networks_with_no_matching_genes()
{
    NeuralNetwork network1({5, 10, 2});
    NeuralNetwork network2({5, 10, 2});
    for (const int inno : {0, 1, 2, 3, 4, 5, 6, 7, 8})
    {
        network1.disable_connection(inno);
    }

    for (const auto &[inno, conn] : network2.get_conn_refs())
    {
        if (inno > 8)
        {
            network2.disable_connection(inno);
        }
    }

    NeuralNetwork network3 = network1 * network2;
    const auto &p1_refs = network1.get_conn_refs();
    const auto &p2_refs = network2.get_conn_refs();
    const bool from_p1 = p1_refs.size() == network3.get_conn_refs().size();
    for (const auto &[inno, conn] : network3.get_conn_refs())
    {
        if (from_p1 && !p1_refs.at(inno).enabled && p1_refs.at(inno).weight != conn.weight &&
            p1_refs.at(inno).from.id != conn.from.id)
        {
            std::cout << ": FAILED - expected a gene from parent 1 and it was either not here or had the attributes";
            return false;
        }

        if (!from_p1 && !p2_refs.at(inno).enabled && p2_refs.at(inno).weight != conn.weight &&
            p2_refs.at(inno).from.id != conn.from.id)
        {
            std::cout << ": FAILED - expected a gene from parent 2 and it was either not here or had the attributes";
            return false;
        }
    }

    if (network3.get_max_innovation() != 8 && network3.get_max_innovation() != 69)
    {
        std::cout << ": FAILED - wrong max innovation value";
        return false;
    }

    std::cout << ": PASSED";
    return true;
}

bool compare_identical_networks()
{
    NeuralNetwork network({5, 10, 2});
    if (network == network)
    {
        std::cout << ": PASSED";
        return true;
    }

    std::cout << ": FAILED - identical networks should be in the same species";
    return false;
}

bool compare_networks_with_same_structure()
{
    NeuralNetwork network1({5, 10, 2});
    NeuralNetwork network2({5, 10, 2});
    if (network1 == network2)
    {
        std::cout << ": PASSED";
        return true;
    }

    std::cout << ": FAILED - same structure networks should probably be in the same species";
    return false;
}

bool compare_networks_where_network1_is_a_subset_of_the_other()
{
    NeuralNetwork network1({4, 2});
    NeuralNetwork network2({4, 2});
    for (const auto &[inno, conn] : network1.get_conn_refs())
    {
        if (inno < 3)
        {
            network1.disable_connection(inno);
        }
    }

    if (network1 == network2)
    {
        std::cout << ": FAILED - subset structure networks should probably not be in the same species";
        return false;
    }

    std::cout << ": PASSED";
    return true;
}

bool compare_networks_where_network2_is_a_subset_of_the_other()
{
    NeuralNetwork network1({4, 2});
    NeuralNetwork network2({4, 2});
    for (const auto &[inno, conn] : network2.get_conn_refs())
    {
        if (inno != 3)
        {
            network2.disable_connection(inno);
        }
    }

    if (network1 == network2)
    {
        std::cout << ": FAILED - subset structure networks should probably not be in the same species";
        return false;
    }

    std::cout << ": PASSED";
    return true;
}

bool compare_networks_with_no_matching_genes()
{
    NeuralNetwork network1({4, 2});
    NeuralNetwork network2({4, 2});
    for (const auto &[inno, conn] : network1.get_conn_refs())
    {
        if (inno < 5)
        {
            network1.disable_connection(inno);
        }
    }

    for (const auto &[inno, conn] : network2.get_conn_refs())
    {
        if (inno >= 5)
        {
            network2.disable_connection(inno);
        }
    }

    if (network1 == network2)
    {
        std::cout << ": FAILED - networks with no matching genes should probably not be in the same species";
        return false;
    }

    std::cout << ": PASSED";
    return true;
}

bool activate_one_to_one()
{ return false; }

bool activate_one_to_many()
{ return false; }

bool activate_many_to_one()
{ return false; }

bool activate_many_to_many()
{ return false; }

bool activate_with_one_hidden_layer()
{ return false; }

bool activate_with_many_hidden_layers()
{ return false; }

int main()
{
    /** TODO: what I've gained from doing these tests
     *    - Made me realise crossover function was bad as it would duplicate both nodes and connections. This was fixed
     *      by adding in the default constructor which I forgot I still didn't add (this would then let me build the
     *      child network from scratch) along with also keeping track of what nodes have been added so that I don't add
     *      them again
     *    - Let me find a major error with how I was handling references to both nodes and connections within vectors.
     *    - Made me aware that I wasn't checking for if a connection was enabled in both the crossover and '==' methods
     *      which they should have been
     *    - Made me realise that there was more balancing to be done for coefficients in the '==' method
     *    - Found divide by zero error in '==' method
     */

    std::cout << "Neural network testing\n";
    std::vector<bool (*)()> tests{create_neural_net_with_one_layer, create_neural_net_with_input_and_output_layers,
                                  create_neural_net_with_one_hidden_layer, create_neural_net_with_many_hidden_layers,
                                  add_node_with_random_bias_and_bad_layer, add_node_with_random_bias_and_good_layer,
                                  add_node_with_given_bias_and_bad_layer, add_node_with_given_bias_and_good_layer,
                                  make_space_between_with_layers_being_equal,
                                  make_space_between_with_layer1_being_greater_than_layer2,
                                  make_space_between_with_layer1_being_negative,
                                  make_space_between_with_layer1_not_existing,
                                  make_space_between_with_layer2_not_existing,
                                  make_space_between_an_odd_number_of_layers,
                                  make_space_between_an_even_number_of_layers,
                                  make_space_between_with_layer1_and_layer2_being_next_to_each_other,
                                  add_connection_with_random_weight_and_bad_innovation,
                                  add_connection_with_random_weight_and_good_innovation,
                                  add_connection_with_given_weight_and_bad_innovation,
                                  add_connection_with_given_weight_and_good_innovation,
                                  check_mutate_weights_just_changes_the_weights,
                                  check_mutate_weights_leaves_disabled_connections, crossover_identical_networks,
                                  crossover_networks_with_same_structure,
                                  crossover_networks_where_network1_is_a_subset_of_the_other,
                                  crossover_networks_where_network2_is_a_subset_of_the_other,
                                  crossover_networks_with_some_disjoint_and_excess_genes,
                                  crossover_networks_with_no_matching_genes, compare_identical_networks,
                                  compare_networks_with_same_structure,
                                  compare_networks_where_network1_is_a_subset_of_the_other,
                                  compare_networks_where_network2_is_a_subset_of_the_other,
                                  compare_networks_with_no_matching_genes};
    int passed = 0;
    for (int test = 0; test < static_cast<int>(tests.size()); ++test)
    {
        std::cout << "TEST " << test + 1;
        passed += tests[test]();  // TODO: I don't like this syntax, we need to find C++'s equivalent to Python's enumerate
        std::cout << "\n";
    }

    std::cout << passed << '/' << tests.size() << " Tests passed.\n";

    return 0;
}

bool check_creation(const std::vector<int> &layer_counts)
{
    NeuralNetwork network;
    try
    {
        network = NeuralNetwork(layer_counts);
    }
    catch (const std::invalid_argument &error)
    {
        std::cout << ": FAILED - constructor failed to create the network";
        return false;
    }

    return has_correct_structure(network, layer_counts);
}

bool has_correct_structure(const NeuralNetwork &network, const std::vector<int> &layer_counts)
{
    unsigned int expected_conn_count = 0;
    for (int layer = 0; layer < static_cast<int>(layer_counts.size()) - 1; ++layer)
    {
        expected_conn_count += layer_counts[layer] * layer_counts[layer + 1];
    }

    if (network.get_conn_refs().size() != expected_conn_count)
    {
        std::cout << ": FAILED - wrong number of connections";
        return false;
    }

    for (const auto &[inno, conn] : network.get_conn_refs())
    {
        if (conn.from.layer != conn.to.layer - 1)
        {
            std::cout << ": FAILED - bad connection";
            return false;
        }
    }

    std::cout << ": PASSED";
    return true;
}

bool bad_make_space_call(NeuralNetwork &network, const int layer1, const int layer2)
{
    try
    {
        network.make_space_between(layer1, layer2);
    }
    catch (const std::invalid_argument &error)
    {
        std::cout << ": PASSED";
        return true;
    }

    std::cout << ": FAILED - accepted NeuralNetwork::make_space_between being called with bad arguments";
    return false;
}

bool find_layer_make_space_call(NeuralNetwork &network, const int layer1, const int layer2, const int expected_layer)
{
    auto old_size = network.get_nodes().size();
    int found_layer = network.make_space_between(layer1, layer2);
    if (found_layer != expected_layer)
    {
        std::cout << ": FAILED - provided wrong layer in-between";
        return false;
    }

    if (network.get_nodes().size() != old_size)
    {
        std::cout << ": FAILED - unnecessary layer added";
        return false;
    }

    std::cout << ": PASSED";
    return true;
}
