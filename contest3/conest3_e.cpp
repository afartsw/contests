#include <iostream>
#include <vector>
#include <algorithm>

int n, m;
std::vector<std::vector<int>> g, grev;
std::vector<bool> used;
std::vector<int> order, comp;

void dfs1(int v) {
    used[v] = true;
    for(int i = 0; i < g[v].size(); i++) {
        int to = g[v][i];
        if(!used[to]) dfs1(to);
    }
    order.push_back(v);
}

void dfs2(int v, int c) {
    used[v] = true;
    comp[v] = c;
    for(int i = 0; i < grev[v].size(); i++) {
        int to = grev[v][i];
        if(!used[to]) dfs2(to, c);
    }
}

int main() {
    std::cin >> n >> m;
    g.resize(n + 1);
    grev.resize(n + 1);

    for(int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        g[u].push_back(v);
        grev[v].push_back(u);
    }

    used.assign(n + 1, false);
    for(int i = 1; i <= n; i++) {
        if(!used[i]) dfs1(i);
    }

    used.assign(n + 1, false);
    comp.resize(n + 1);
    int c = 0;

    for(int i = n - 1; i >= 0; i--) {
        int v = order[i];
        if(!used[v]) {
            c++;
            dfs2(v, c);
        }
    }

    std::cout << c << "\n";
    for(int i = 1; i <= n; i++) {
        std::cout << comp[i] << " ";
    }
    std::cout << "\n";

    return 0;
}
