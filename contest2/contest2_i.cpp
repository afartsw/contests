#include <iostream>
#include <vector>
#include <string>

std::vector<std::vector<long long>> multiply(const std::vector<std::vector<long long>>& a, const std::vector<std::vector<long long>>& b, long long mod) {
    int size = a.size();
    std::vector<std::vector<long long>> result(size, std::vector<long long>(size, 0));
    for (int i = 0; i < size; ++i) {
        for (int k = 0; k < size; ++k) {
            if (a[i][k] == 0) continue;
            for (int j = 0; j < size; ++j) {
                result[i][j] = (result[i][j] + a[i][k] * b[k][j]) % mod;
            }
        }
    }
    return result;
}

std::vector<std::vector<long long>> power(std::vector<std::vector<long long>> base, long long p, long long mod) {
    int size = base.size();
    std::vector<std::vector<long long>> result(size, std::vector<long long>(size, 0));
    for (int i = 0; i < size; ++i) {
        result[i][i] = 1;
    }
    while (p > 0) {
        if (p % 2 == 1) {
            result = multiply(result, base, mod);
        }
        base = multiply(base, base, mod);
        p /= 2;
    }
    return result;
}

void decrement_string(std::string& s) {
    int i = s.size() - 1;
    while (i >= 0 && s[i] == '0') {
        s[i] = '9';
        i--;
    }
    if (i >= 0) {
        s[i]--;
    }
    if (s.size() > 1 && s[0] == '0') {
        s.erase(0, 1);
    }
}

int main() {
    std::string n_str;
    int m;
    long long mod;

    if (!(std::cin >> n_str >> m >> mod)) return 0;

    decrement_string(n_str);
    if (n_str == "0") {
        long long total = 1;
        for (int i = 0; i < m; ++i) total = (total * 2) % mod;
        std::cout << total << "\n";
        return 0;
    }

    int total_masks = 1 << m;

    std::vector<std::vector<long long>> transition(total_masks, std::vector<long long>(total_masks, 0));

    for (int prev_mask = 0; prev_mask < total_masks; ++prev_mask) {
        for (int curr_mask = 0; curr_mask < total_masks; ++curr_mask) {
            bool flag = true;
            for (int row = 0; row < m - 1; ++row) {
                int top_left = (prev_mask >> row) & 1;
                int bottom_left = (prev_mask >> (row + 1)) & 1;
                int top_right = (curr_mask >> row) & 1;
                int bottom_right = (curr_mask >> (row + 1)) & 1;
                if (top_left == bottom_left && bottom_left == top_right && top_right == bottom_right) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                transition[prev_mask][curr_mask] = 1;
            }
        }
    }

    std::vector<std::vector<long long>> result_matrix(total_masks, std::vector<long long>(total_masks, 0));
    for (int i = 0; i < total_masks; ++i) {
        result_matrix[i][i] = 1;
    }

    for (std::string::size_type idx = 0; idx < n_str.size(); ++idx) {
        int d = n_str[idx] - '0';
        result_matrix = power(result_matrix, 10, mod);
        std::vector<std::vector<long long>> digit_matrix = power(transition, d, mod);
        result_matrix = multiply(result_matrix, digit_matrix, mod);
    }

    std::vector<long long> dp(total_masks, 1);
    std::vector<long long> final_dp(total_masks, 0);

    for (int i = 0; i < total_masks; ++i) {
        long long sum = 0;
        for (int j = 0; j < total_masks; ++j) {
            sum = (sum + result_matrix[i][j] * dp[j]) % mod;
        }
        final_dp[i] = sum;
    }

    long long total_ways = 0;
    for (int i = 0; i < total_masks; ++i) {
        total_ways = (total_ways + final_dp[i]) % mod;
    }

    std::cout << total_ways << "\n";

    return 0;
}
