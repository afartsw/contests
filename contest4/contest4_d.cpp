#include <iostream>
#include <vector>
#include <algorithm>

struct Edge {
    int server_from;
    int server_to;
    int capacity;

    bool operator<(const Edge& other) const {
        return capacity > other.capacity;
    }
};

class DSU {
private:
    std::vector<int> parent;
    std::vector<int> rank;

public:
    explicit DSU(int nodes_cnt) {
        parent.resize(nodes_cnt + 1);
        rank.assign(nodes_cnt + 1, 0);
        for (int node_index = 1; node_index <= nodes_cnt; ++node_index) {
            parent[node_index] = node_index;
        }
    }

    int FindRoot(int node) {
        if (node == parent[node]) {
            return node;
        }
        return parent[node] = FindRoot(parent[node]);
    }

    bool Unite(int first_node, int second_node) {
        int root_first = FindRoot(first_node);
        int root_second = FindRoot(second_node);

        if (root_first != root_second) {
            if (rank[root_first] < rank[root_second]) {
                std::swap(root_first, root_second);
            }
            parent[root_second] = root_first;
            if (rank[root_first] == rank[root_second]) {
                rank[root_first]++;
            }
            return true;
        }
        return false;
    }
};

struct TreeEdge {
    int target;
    int weight;
};

const int MAX_LOG = 12;

void DFS(
    int current_node, int parent_node, int current_depth, int edge_weight,
    const std::vector<std::vector<TreeEdge>>& graph,
    std::vector<int>& depths,
    std::vector<std::vector<int>>& ancestors,
    std::vector<std::vector<int>>& min_table) {

    depths[current_node] = current_depth;
    ancestors[current_node][0] = parent_node;
    min_table[current_node][0] = edge_weight;

    for (int step = 1; step <= MAX_LOG; ++step) {
        int intermediate = ancestors[current_node][step - 1];
        ancestors[current_node][step] = ancestors[intermediate][step - 1];
        min_table[current_node][step] = std::min(
            min_table[current_node][step - 1],
            min_table[intermediate][step - 1]
        );
    }

    for (const auto& edge : graph[current_node]) {
        if (edge.target != parent_node) {
            DFS(edge.target, current_node, current_depth + 1, edge.weight,
                graph, depths, ancestors, min_table);
        }
    }
}

int GetMinCapacity(
    int first_node, int second_node,
    const std::vector<int>& depths,
    const std::vector<std::vector<int>>& ancestors,
    const std::vector<std::vector<int>>& min_table) {

    int min_capacity = 1e9;

    if (depths[first_node] < depths[second_node]) {
        std::swap(first_node, second_node);
    }

    for (int step = MAX_LOG; step >= 0; --step) {
        if (depths[first_node] - (1 << step) >= depths[second_node]) {
            min_capacity = std::min(min_capacity, min_table[first_node][step]);
            first_node = ancestors[first_node][step];
        }
    }

    if (first_node == second_node) {
        return min_capacity;
    }

    for (int step = MAX_LOG; step >= 0; --step) {
        if (ancestors[first_node][step] != ancestors[second_node][step]) {
            min_capacity = std::min(min_capacity, min_table[first_node][step]);
            min_capacity = std::min(min_capacity, min_table[second_node][step]);
            first_node =  ancestors[first_node][step];
            second_node = ancestors[second_node][step];
        }
    }

    min_capacity = std::min(min_capacity, min_table[first_node][0]);
    min_capacity = std::min(min_capacity, min_table[second_node][0]);

    return min_capacity;
}

int main() {
    int total_servers, total_channels, total_queries;
    if (!(std::cin >> total_servers >> total_channels >> total_queries)) {
        return 0;
    }

    std::vector<Edge> edges(total_channels);
    for (int channel_index = 0; channel_index < total_channels; ++channel_index) {
        std::cin >> edges[channel_index].server_from
                 >> edges[channel_index].server_to
                 >> edges[channel_index].capacity;
    }

    std::sort(edges.begin(), edges.end());
    DSU dsu(total_servers);
    std::vector<std::vector<TreeEdge>> graph(total_servers + 1);

    int edges_added = 0;
    for (const auto& channel : edges) {
        if (dsu.Unite(channel.server_from, channel.server_to)) {
            graph[channel.server_from].push_back({channel.server_to, channel.capacity});
            graph[channel.server_to].push_back({channel.server_from, channel.capacity});
            edges_added++;
            if (edges_added == total_servers - 1) {
                break;
            }
        }
    }

    std::vector<int> depths(total_servers + 1, 0);
    std::vector<std::vector<int>> ancestors(
        total_servers + 1, std::vector<int>(MAX_LOG + 1, 0));
    std::vector<std::vector<int>> min_table(
        total_servers + 1, std::vector<int>(MAX_LOG + 1, 1e9));

    DFS(1, 1, 0, 1e9, graph, depths, ancestors, min_table);

    for (int query_index = 0; query_index < total_queries; ++query_index) {
        int server_source, server_destination;
        std::cin >> server_source >> server_destination;

        int max_bandwidth = GetMinCapacity(
            server_source, server_destination,
            depths, ancestors, min_table);

        std::cout << max_bandwidth << "\n";
    }

    return 0;
}
