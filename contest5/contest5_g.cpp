#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

typedef unsigned long long BoardMask;

struct QueueState {
    int f_score;
    int g_score;
    int zero_pos;
    BoardMask mask;

    bool operator>(const QueueState& other) const {
        return f_score > other.f_score;
    }
};

struct MoveTrace {
    BoardMask prev_mask;
    char move;
    bool has_prev;
};

int ReadTile(BoardMask mask, int cell_pos) {
    return static_cast<int>((mask >> (cell_pos * 4)) & 0xFULL);
}

BoardMask WriteTile(BoardMask mask, int cell_pos, int tile_val) {
    int shift = cell_pos * 4;
    BoardMask cleared = mask & ~(0xFULL << shift);
    return cleared | (static_cast<BoardMask>(tile_val) << shift);
}

BoardMask EncodeBoard(const std::vector<int>& tiles) {
    BoardMask mask = 0;
    for (size_t idx = 0; idx < tiles.size(); ++idx) {
        mask = WriteTile(mask, static_cast<int>(idx), tiles[idx]);
    }
    return mask;
}

bool IsInsideBoard(int row_idx, int col_idx, int board_len) {
    return row_idx >= 0 && row_idx < board_len &&
           col_idx >= 0 && col_idx < board_len;
}

bool CanSolve(const std::vector<int>& tiles, int zero_pos, int board_len) {
    int total = board_len * board_len;
    int inv = 0;
    for (int first_idx = 0; first_idx < total; ++first_idx) {
        if (tiles[first_idx] == 0) {
            continue;
        }
        for (int second_idx = first_idx + 1; second_idx < total; ++second_idx) {
            if (tiles[second_idx] != 0 && tiles[first_idx] > tiles[second_idx]) {
                ++inv;
            }
        }
    }
    int zero_row = zero_pos / board_len + 1;
    return (inv + zero_row) % 2 == 0;
}

int EstimateCost(BoardMask mask, int board_len) {
    int total = board_len * board_len;
    int manhattan = 0;

    for (int cell_pos = 0; cell_pos < total; ++cell_pos) {
        int tile = ReadTile(mask, cell_pos);
        if (tile == 0) {
            continue;
        }
        int target = tile - 1;
        manhattan += std::abs(cell_pos / board_len - target / board_len) +
                     std::abs(cell_pos % board_len - target % board_len);
    }

    int conflicts = 0;

    for (int row = 0; row < board_len; ++row) {
        for (int col_one = 0; col_one < board_len; ++col_one) {
            int tile_one = ReadTile(mask, row * board_len + col_one);
            if (tile_one == 0 || (tile_one - 1) / board_len != row) {
                continue;
            }
            for (int col_two = col_one + 1; col_two < board_len; ++col_two) {
                int tile_two = ReadTile(mask, row * board_len + col_two);
                if (tile_two == 0 || (tile_two - 1) / board_len != row) {
                    continue;
                }
                if (tile_one > tile_two) {
                    conflicts += 2;
                }
            }
        }
    }

    for (int col = 0; col < board_len; ++col) {
        for (int row_one = 0; row_one < board_len; ++row_one) {
            int tile_one = ReadTile(mask, row_one * board_len + col);
            if (tile_one == 0 || (tile_one - 1) % board_len != col) {
                continue;
            }
            for (int row_two = row_one + 1; row_two < board_len; ++row_two) {
                int tile_two = ReadTile(mask, row_two * board_len + col);
                if (tile_two == 0 || (tile_two - 1) % board_len != col) {
                    continue;
                }
                if ((tile_one - 1) / board_len > (tile_two - 1) / board_len) {
                    conflicts += 2;
                }
            }
        }
    }

    return manhattan + conflicts;
}

void SolveFifteen(const std::vector<int>& start_tiles, int start_zero,
                  int board_len) {
    if (!CanSolve(start_tiles, start_zero, board_len)) {
        std::cout << -1 << "\n";
        return;
    }

    int total = board_len * board_len;
    std::vector<int> goal_tiles(total, 0);
    for (int idx = 0; idx < total - 1; ++idx) {
        goal_tiles[idx] = idx + 1;
    }

    BoardMask start_mask = EncodeBoard(start_tiles);
    BoardMask goal_mask = EncodeBoard(goal_tiles);

    if (start_mask == goal_mask) {
        std::cout << 0 << "\n\n";
        return;
    }

    std::priority_queue<QueueState, std::vector<QueueState>,
                        std::greater<QueueState>> open_queue;
    std::unordered_map<BoardMask, int> min_g;
    std::unordered_map<BoardMask, MoveTrace> trace;

    QueueState init;
    init.g_score = 0;
    init.zero_pos = start_zero;
    init.mask = start_mask;
    init.f_score = EstimateCost(start_mask, board_len);

    min_g[start_mask] = 0;
    MoveTrace root;
    root.prev_mask = 0;
    root.move = 0;
    root.has_prev = false;
    trace[start_mask] = root;
    open_queue.push(init);

    int delta_row[4] = {0, 0, 1, -1};
    int delta_col[4] = {1, -1, 0, 0};
    char dirs[4] = {'L', 'R', 'U', 'D'};

    while (!open_queue.empty()) {
        QueueState cur = open_queue.top();
        open_queue.pop();

        std::unordered_map<BoardMask, int>::iterator best_it = min_g.find(cur.mask);
        if (best_it != min_g.end() && cur.g_score > best_it->second) {
            continue;
        }

        if (cur.mask == goal_mask) {
            std::string path;
            BoardMask cur_mask = goal_mask;
            while (true) {
                MoveTrace step = trace[cur_mask];
                if (!step.has_prev) {
                    break;
                }
                path.push_back(step.move);
                cur_mask = step.prev_mask;
            }
            std::reverse(path.begin(), path.end());
            std::cout << path.size() << "\n" << path << "\n";
            return;
        }

        int zero_row = cur.zero_pos / board_len;
        int zero_col = cur.zero_pos % board_len;
        int next_g = cur.g_score + 1;

        for (int dir_idx = 0; dir_idx < 4; ++dir_idx) {
            int next_row = zero_row + delta_row[dir_idx];
            int next_col = zero_col + delta_col[dir_idx];
            if (!IsInsideBoard(next_row, next_col, board_len)) {
                continue;
            }

            int next_zero = next_row * board_len + next_col;
            int moved = ReadTile(cur.mask, next_zero);

            BoardMask next_mask = WriteTile(cur.mask, cur.zero_pos, moved);
            next_mask = WriteTile(next_mask, next_zero, 0);

            std::unordered_map<BoardMask, int>::iterator prev_it = min_g.find(next_mask);
            if (prev_it != min_g.end() && next_g >= prev_it->second) {
                continue;
            }

            min_g[next_mask] = next_g;

            MoveTrace next_step;
            next_step.prev_mask = cur.mask;
            next_step.move = dirs[dir_idx];
            next_step.has_prev = true;
            trace[next_mask] = next_step;

            QueueState next_state;
            next_state.g_score = next_g;
            next_state.zero_pos = next_zero;
            next_state.mask = next_mask;
            next_state.f_score = next_g + EstimateCost(next_mask, board_len);
            open_queue.push(next_state);
        }
    }

    std::cout << -1 << "\n";
}

int main() {
    int board_len = 4;
    int total = board_len * board_len;
    std::vector<int> tiles(total, 0);
    int zero_pos = -1;

    for (int idx = 0; idx < total; ++idx) {
        std::cin >> tiles[idx];
        if (tiles[idx] == 0) {
            zero_pos = idx;
        }
    }

    SolveFifteen(tiles, zero_pos, board_len);
    return 0;
}
