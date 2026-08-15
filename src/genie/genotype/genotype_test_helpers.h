/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_GENOTYPE_GENOTYPE_TEST_HELPERS_H_
#define SRC_GENIE_GENOTYPE_GENOTYPE_TEST_HELPERS_H_

#include "genie/genotype/genotype_coder.h"
#include <algorithm>
#include <vector>
#include <random>
#include <cmath>
#include <numeric>
#include <utility>

namespace genie::genotype {

template <typename T>
auto random_matrix(size_t nrows, size_t ncols, T min, T max) {
    std::vector<std::vector<T>> mat(nrows, std::vector<T>(ncols));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int32_t> dis(static_cast<int32_t>(min), static_cast<int32_t>(max) - 1);
    for (size_t i = 0; i < nrows; ++i) {
        for (size_t j = 0; j < ncols; ++j) {
            mat[i][j] = static_cast<T>(dis(gen));
        }
    }
    return mat;
}

template <typename T>
auto create_matrix(std::initializer_list<std::initializer_list<T>> data) {
    std::vector<std::vector<T>> mat;
    for (auto row : data) mat.push_back(row);
    return mat;
}

template <typename T>
bool equal(const std::vector<std::vector<T>>& m1, const std::vector<std::vector<T>>& m2) {
    return m1 == m2;
}

template <typename T>
size_t get_nrows(const std::vector<std::vector<T>>& mat) {
    return mat.size();
}

template <typename T>
size_t get_ncols(const std::vector<std::vector<T>>& mat) {
    return mat.empty() ? 0 : mat[0].size();
}

template <typename T, typename V>
void set_element(std::vector<std::vector<T>>& mat, size_t i, size_t j, V value) {
    mat[i][j] = static_cast<T>(value);
}

template <typename T, typename V>
void set_by_mask(std::vector<std::vector<T>>& mat, const std::vector<std::vector<bool>>& mask, V value) {
    for (size_t i = 0; i < mat.size(); ++i) {
        for (size_t j = 0; j < mat[i].size(); ++j) {
            if (mask[i][j]) mat[i][j] = static_cast<T>(value);
        }
    }
}

template <typename T>
auto argsort(const std::vector<T>& v) {
    std::vector<uint32_t> indices(v.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::stable_sort(indices.begin(), indices.end(),
              [&v](uint32_t i1, uint32_t i2) { return v[i1] < v[i2]; });
    return indices;
}

template <typename T>
auto sum(const std::vector<T>& v) {
    return std::accumulate(v.begin(), v.end(), 0ULL);
}

template <typename T>
auto sum(const std::vector<std::vector<T>>& mat) {
    uint64_t s = 0;
    for (const auto& row : mat) {
        for (const auto& val : row) s += static_cast<uint64_t>(val);
    }
    return s;
}

template <typename T>
auto amax(const std::vector<std::vector<T>>& mat) {
    if (mat.empty() || mat[0].empty()) return static_cast<T>(0);  // Should handle empty case safely
    T m = mat[0][0];
    for (const auto& row : mat) {
        for (const auto& val : row) if (val > m) m = val;
    }
    return m;
}

template <typename T>
auto amin(const std::vector<std::vector<T>>& mat) {
    if (mat.empty() || mat[0].empty()) return static_cast<T>(0);  // Should handle empty case safely
    T m = mat[0][0];
    for (const auto& row : mat) {
        for (const auto& val : row) if (val < m) m = val;
    }
    return m;
}

template <typename Target, typename Source>
auto cast(const std::vector<Source>& data) {
    std::vector<Target> result;
    result.reserve(data.size());
    for (const auto& val : data) {
        result.push_back(static_cast<Target>(val));
    }
    return result;
}

template <typename Target, typename Source>
auto cast(const std::vector<std::vector<Source>>& data) {
    std::vector<std::vector<Target>> result;
    result.reserve(data.size());
    for (const auto& row : data) {
        std::vector<Target> new_row;
        new_row.reserve(row.size());
        for (const auto& val : row) {
            new_row.push_back(static_cast<Target>(val));
        }
        result.push_back(std::move(new_row));
    }
    return result;
}

template <typename T>
auto linspace(T start, T end, size_t num) {
    std::vector<T> result;
    result.reserve(num);
    if (num == 0) return result;
    if (num == 1) {
        result.push_back(start);
        return result;
    }
    double step = static_cast<double>(end - start) / (num - 1);
    for (size_t i = 0; i < num; ++i) {
        result.push_back(static_cast<T>(start + i * step));
    }
    return result;
}

template <typename T>
auto random_permutation(T n) {
    std::vector<T> result(n);
    std::iota(result.begin(), result.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(result.begin(), result.end(), g);
    return result;
}

}  // namespace genie::genotype

namespace std {
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << v[i];
        if (i != v.size() - 1) os << ", ";
    }
    os << "]";
    return os;
}
}  // namespace std

#endif  // SRC_GENIE_GENOTYPE_GENOTYPE_TEST_HELPERS_H_
