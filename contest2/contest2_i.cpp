#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using Matrix = std::vector<std::vector<long long>>;

Matrix MultiplyMatrices(const Matrix& matrix_a,
                        const Matrix& matrix_b,
                        long long mod) {
    size_t size = matrix_a.size();
    Matrix result(size, std::vector<long long>(size, 0));

    for (size_t i = 0; i < size; ++i) {
        for (size_t k = 0; k < size; ++k) {
            if (matrix_a[i][k] == 0) {
                continue;
            }
            for (size_t j = 0; j < size; ++j) {
                result[i][j] = (result[i][j] + matrix_a[i][k] * matrix_b[k][j]) % mod;
            }
        }
    }
    return result;
}

Matrix PowerMatrix(Matrix base, long long power_value, long long mod) {
    size_t size = base.size();
    Matrix result(size, std::vector<long long>(size, 0));

    for (size_t i = 0; i < size; ++i) {
        result[i][i] = 1;
    }

    while (power_value > 0) {
        if (power_value % 2 == 1) {
            result = MultiplyMatrices(result, base, mod);
        }
        base = MultiplyMatrices(base, base, mod);
        power_value /= 2;
    }
    return result;
}

void DecrementString(std::string& s) {
    if (s == "0") {
        return;
    }
    int i = static_cast<int>(s.size()) - 1;
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

bool HasMonochromeSquare(int prev_mask, int curr_mask, int num_rows) {
    for (int row = 0; row < num_rows - 1; ++row) {
        int top_left = (prev_mask >> row) & 1;
        int bottom_left = (prev_mask >> (row + 1)) & 1;
        int top_right = (curr_mask >> row) & 1;
        int bottom_right = (curr_mask >> (row + 1)) & 1;

        if (top_left == bottom_left && bottom_left == top_right && top_right == bottom_right) {
            return true;
        }
    }
    return false;
}

Matrix BuildTransitionMatrix(int num_rows) {
    int total_masks = 1 << num_rows;
    Matrix transition(total_masks, std::vector<long long>(total_masks, 0));

    for (int prev_mask = 0; prev_mask < total_masks; ++prev_mask) {
        for (int curr_mask = 0; curr_mask < total_masks; ++curr_mask) {
            if (!HasMonochromeSquare(prev_mask, curr_mask, num_rows)) {
                transition[prev_mask][curr_mask] = 1;
            }
        }
    }
    return transition;
}

long long HandleZeroCase(int num_rows, long long mod) {
    long long total = 1 % mod;
    for (int i = 0; i < num_rows; ++i) {
        total = (total << 1) % mod;
    }
    return total;
}

long long ComputeResult(std::string n_str, int num_rows, long long mod) {
    if (n_str == "0") {
        return HandleZeroCase(num_rows, mod);
    }

    DecrementString(n_str);

    int total_masks = 1 << num_rows;
    Matrix transition = BuildTransitionMatrix(num_rows);

    Matrix result_matrix(total_masks, std::vector<long long>(total_masks, 0));
    for (int i = 0; i < total_masks; ++i) {
        result_matrix[i][i] = 1;
    }

    for (std::string::size_type idx = 0; idx < n_str.size(); ++idx) {
        int digit = n_str[idx] - '0';
        result_matrix = PowerMatrix(result_matrix, 10, mod);
        Matrix digit_matrix = PowerMatrix(transition, digit, mod);
        result_matrix = MultiplyMatrices(result_matrix, digit_matrix, mod);
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

    return total_ways;
}

void PrintResult(long long answer) {
    std::cout << answer << "\n";
}

int main() {
    std::string n_str;
    int num_rows;
    long long mod;

    if (!(std::cin >> n_str >> num_rows >> mod)) return 0;

    long long answer = ComputeResult(n_str, num_rows, mod);
    PrintResult(answer);

    return 0;
}
