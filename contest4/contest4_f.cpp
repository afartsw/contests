#include <algorithm>
#include <iostream>
#include <vector>

const int MAX_LOG = 17;

struct Edge {
  int to;
  int id;
};

struct BridgeData {
  std::vector<int> tin;
  std::vector<int> low;
  std::vector<bool> is_bridge;
  int timer;
};

struct LCAData {
  std::vector<std::vector<int>> tree;
  std::vector<int> depth;
  std::vector<std::vector<int>> up;
};

void FindBridgesDFS(int vertex, int parent_edge,
                    const std::vector<std::vector<Edge>>& adj,
                    BridgeData& data) {
  data.tin[vertex] = data.low[vertex] = ++data.timer;
  for (const auto& edge : adj[vertex]) {
    if (edge.id == parent_edge) continue;

    if (data.tin[edge.to] != 0) {
      data.low[vertex] = std::min(data.low[vertex], data.tin[edge.to]);
    } else {
      FindBridgesDFS(edge.to, edge.id, adj, data);
      data.low[vertex] = std::min(data.low[vertex], data.low[edge.to]);
      if (data.low[edge.to] > data.tin[vertex]) {
        data.is_bridge[edge.id] = true;
      }
    }
  }
}

void ColourComponents(int vertex, int current_comp,
                         const std::vector<std::vector<Edge>>& adj,
                         const std::vector<bool>& is_bridge,
                         std::vector<int>& comp_id) {
  comp_id[vertex] = current_comp;
  for (const auto& edge : adj[vertex]) {
    if (comp_id[edge.to] == 0 && !is_bridge[edge.id]) {
      ColourComponents(edge.to, current_comp, adj, is_bridge, comp_id);
    }
  }
}

void BuildLCADFS(int vertex, int parent, int current_depth, LCAData& lca) {
  lca.depth[vertex] = current_depth;
  lca.up[vertex][0] = parent;
  for (int step = 1; step < MAX_LOG; ++step) {
    lca.up[vertex][step] = lca.up[lca.up[vertex][step - 1]][step - 1];
  }
  for (int neighbor : lca.tree[vertex]) {
    if (neighbor != parent) {
      BuildLCADFS(neighbor, vertex, current_depth + 1, lca);
    }
  }
}

int GetLCA(int first, int second, const LCAData& lca) {
  if (lca.depth[first] < lca.depth[second]) std::swap(first, second);

  for (int step = MAX_LOG - 1; step >= 0; --step) {
    if (lca.depth[first] - (1 << step) >= lca.depth[second]) {
      first = lca.up[first][step];
    }
  }
  if (first == second) {
    return first;
  }
  for (int step = MAX_LOG - 1; step >= 0; --step) {
    if (lca.up[first][step] != lca.up[second][step]) {
      first = lca.up[first][step];
      second = lca.up[second][step];
    }
  }
  return lca.up[first][0];
}

void ReadInput(int& city_count, int& road_count, int& finish_node,
               std::vector<std::vector<Edge>>& adj) {
  std::cin >> city_count >> road_count;
  std::cin >> finish_node;

  adj.resize(city_count + 1);
  for (int index = 0; index < road_count; ++index) {
    int first_city = 0;
    int second_city = 0;
    std::cin >> first_city >> second_city;
    adj[first_city].push_back({second_city, index});
    adj[second_city].push_back({first_city, index});
  }
}

void BuildBridgeTree(int city_count, int road_count, int finish_node,
                     const std::vector<std::vector<Edge>>& adj,
                     std::vector<int>& comp_id, LCAData& lca) {
  BridgeData bridge_data{
      std::vector<int>(city_count + 1, 0),
      std::vector<int>(city_count + 1, 0),
      std::vector<bool>(road_count, false),
      0
  };
  FindBridgesDFS(1, -1, adj, bridge_data);

  comp_id.assign(city_count + 1, 0);
  int comp_count = 0;
  for (int city = 1; city <= city_count; ++city) {
    if (comp_id[city] == 0) {
      ++comp_count;
      ColourComponents(city, comp_count, adj, bridge_data.is_bridge, comp_id);
    }
  }

  lca.tree.resize(comp_count + 1);
  for (int city = 1; city <= city_count; ++city) {
    for (const auto& edge : adj[city]) {
      if (bridge_data.is_bridge[edge.id]) {
        int comp_u = comp_id[city];
        int comp_v = comp_id[edge.to];
        if (comp_u < comp_v) {
          lca.tree[comp_u].push_back(comp_v);
          lca.tree[comp_v].push_back(comp_u);
        }
      }
    }
  }

  lca.depth.assign(comp_count + 1, 0);
  lca.up.assign(comp_count + 1, std::vector<int>(MAX_LOG, 0));

  int root = comp_id[finish_node];
  BuildLCADFS(root, root, 0, lca);
}

void ProcessQueries(const std::vector<int>& comp_id, const LCAData& lca) {
  int query_count = 0;
  std::cin >> query_count;
  for (int query = 0; query < query_count; ++query) {
    int start_first = 0;
    int start_second = 0;
    std::cin >> start_first >> start_second;
    int comp_first = comp_id[start_first];
    int comp_second = comp_id[start_second];
    int ancestor = GetLCA(comp_first, comp_second, lca);
    std::cout << lca.depth[ancestor] << "\n";
  }
}

int main() {
  int city_count = 0;
  int road_count = 0;
  int finish_node = 0;
  std::vector<std::vector<Edge>> adj;

  ReadInput(city_count, road_count, finish_node, adj);

  std::vector<int> comp_id;
  LCAData lca;
  BuildBridgeTree(city_count, road_count, finish_node, adj, comp_id, lca);

  ProcessQueries(comp_id, lca);

  return 0;
}
