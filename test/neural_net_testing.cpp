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
 *   void disable_connection(int inno) - HEADERS DONE
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

void create_neural_net_with_layer_counts_empty()
{}

void create_neural_net_with_one_layer()
{}

void create_neural_net_with_input_and_output_layers()
{}

void create_neural_net_with_one_hidden_layer()
{}

void create_neural_net_with_many_hidden_layers()
{}

void add_node_with_random_bias_and_bad_layer()
{}

void add_node_with_random_bias_and_good_layer()
{}

void add_node_with_given_bias_and_bad_layer()
{}

void add_node_with_given_bias_and_good_layer()
{}

void make_space_between_with_layers_being_equal()
{}

void make_space_between_with_layer1_being_greater_than_layer2()
{}

void make_space_between_with_layer1_being_negative()
{}

void make_space_between_with_layer2_being_less_than_one()
{}

void make_space_between_with_layer1_not_existing()
{}

void make_space_between_with_layer2_not_existing()
{}

void make_space_between_an_odd_number_of_layers()
{}

void make_space_between_an_even_number_of_layers()
{}

void make_space_between_with_layer1_and_layer2_being_next_to_each_other()
{}

void add_connection_with_random_weight_and_bad_innovation()
{}

void add_connection_with_random_weight_and_good_innovation()
{}

void add_connection_with_given_weight_and_bad_innovation()
{}

void add_connection_with_given_weight_and_good_innovation()
{}

// TODO: make sure to have this and the next test run multiple times as we aren't controlling the random probabilities
//   acc we probably will now so should be fine
void check_mutate_weights_keeps_the_net_structure_the_same()
{}

// TODO: for this you can just create a network and disable all of its connections
void check_mutate_weights_leaves_disabled_connections()
{}

void disable_connection_with_inno_that_does_not_exist()
{}

void disable_connection_with_inno_that_does_exist()
{}

void crossover_identical_networks()
{}

void crossover_networks_with_same_structure()
{}

void crossover_networks_where_network1_is_a_subset_of_the_other()
{}

void crossover_networks_where_network2_is_a_subset_of_the_other()
{}

void crossover_networks_with_some_disjoint_and_excess_genes()
{}

void crossover_networks_with_no_matching_genes()
{}

void compare_identical_networks()
{}

void compare_networks_with_same_structure()
{}

void compare_networks_where_network1_is_a_subset_of_the_other()
{}

void compare_networks_where_network2_is_a_subset_of_the_other()
{}

void compare_networks_with_some_disjoint_and_excess_genes()
{}

void compare_networks_with_no_matching_genes()
{}

void activate_one_to_one()
{}

void activate_one_to_many()
{}

void activate_many_to_one()
{}

void activate_many_to_many()
{}

void activate_with_one_hidden_layer()
{}

void activate_with_many_hidden_layers()
{}

int main()
{
    std::cout << "Neural network testing\n";

    return 0;
}
