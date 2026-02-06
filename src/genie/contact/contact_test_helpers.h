/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONTACT_TEST_HELPERS_H
#define GENIE_CONTACT_TEST_HELPERS_H

#include "contact_types.h"
#include <algorithm>
#include <vector>
#include <random>
#include <initializer_list>
#include <iostream>
#include <numeric>

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
}

template <typename T, typename S>
std::vector<T> operator/(const std::vector<T>& a, S b) {
    std::vector<T> res = a;
    for (auto& v : res) v /= b;
    return res;
}

template <typename T, typename S>
std::vector<T> operator*(const std::vector<T>& a, S b) {
    std::vector<T> res = a;
    for (auto& v : res) v *= b;
    return res;
}

template <typename T, typename S>
std::vector<T> operator+(const std::vector<T>& a, S b) {
    std::vector<T> res = a;
    for (auto& v : res) v += b;
    return res;
}

template <typename T, typename S>
std::vector<T> operator-(const std::vector<T>& a, S b) {
    std::vector<T> res = a;
    for (auto& v : res) v -= b;
    return res;
}

namespace genie::contact {

template <typename T>
auto create_vector(const std::vector<T>& data) {
    return data;
}

// Support for initializer_list
template <typename T>
auto create_vector(std::initializer_list<T> data) {
    return std::vector<T>(data);
}

// Specialization for std::vector<bool>
template <>
inline auto create_vector<bool>(const std::vector<bool>& data) {
    return data;
}

template <typename T>
auto create_matrix(std::initializer_list<std::initializer_list<T>> data) {
    std::vector<std::vector<T>> mat;
    for (auto row : data) mat.push_back(row);
    return mat;
}

template <typename T>
auto sum(const T& data) {
    return std::accumulate(data.begin(), data.end(), 0ULL);
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
auto sort(const T& data) {
    auto sorted = data;
    std::sort(sorted.begin(), sorted.end());
    return sorted;
}

template <typename T1, typename T2>
bool equal(const T1& a, const T2& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

template <typename T>
bool all(const T& data) {
    for (const auto& v : data) if (!v) return false;
    return true;
}

inline bool all(bool data) {
    return data;
}

template <typename T>
size_t dims(const T& data) {
    // For std::vector or std::vector<std::vector>>
    // This is hard to detect generically for all depths but we only care about 1 and 2.
    if constexpr (std::is_same_v<T, std::vector<std::vector<typename T::value_type::value_type>>>) {
        return 2;
    }
    return 1; 
}

// Specialization for vector<vector<T>> to get dims correct if T is not nested
template <typename T>
size_t dims(const std::vector<std::vector<T>>&) {
    return 2;
}


template <typename T, typename S>
void add_inplace(T& a, S b) {
    for (auto& v : a) v += b;
}

template <typename T1, typename T2>
auto not_equal(const T1& a, const T2& b) {
    std::vector<bool> result;
    result.reserve(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result.push_back(a[i] != b[i]);
    }
    return result;
}

template <typename T>
auto get(const T& data, size_t i) {
    return data[i];
}

template <typename T>
auto get(const T& data, size_t i, size_t j) {
    return data[i][j];
}

template <typename T>
size_t rows(const T& data) {
    return data.size();
}

template <typename T>
size_t cols(const T& data) {
    if (data.empty()) return 0;
    return data[0].size();
}

template <typename T>
auto random_randint_vec(size_t n, T min_val, T max_val) {
    std::vector<T> vec(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<T> dis(min_val, max_val - 1);
    for (auto& v : vec) v = dis(gen);
    return vec;
}

template <typename T>
auto random_randint_mat(size_t rows, size_t cols, T min_val, T max_val) {
     std::vector<std::vector<T>> mat(rows, std::vector<T>(cols));
     std::random_device rd;
     std::mt19937 gen(rd());
     std::uniform_int_distribution<T> dis(min_val, max_val - 1);
     for (auto& row : mat) for (auto& v : row) v = dis(gen);
     return mat;
}

template <typename T>
auto create_zeros(size_t rows, size_t cols) {
    return std::vector<std::vector<T>>(rows, std::vector<T>(cols, static_cast<T>(0)));
}

template <typename T, typename S>
auto subtract(const T& a, S b) {
    auto res = a;
    for (auto& v : res) v -= b;
    return res;
}

template <typename T, typename S>
auto divide(const T& a, S b) {
    auto res = a;
    for (auto& v : res) v /= b;
    return res;
}

template <typename T>
auto create_zeros(size_t n) {
    return std::vector<T>(n, static_cast<T>(0));
}

template <typename T>
auto create_ones(size_t n) {
    return std::vector<T>(n, static_cast<T>(1));
}

template <typename T, typename S>
auto add(const T& a, S b) {
    auto res = a;
    for (auto& v : res) v += b;
    return res;
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

#define ASSERT_CM_EQUAL(a, b) ASSERT_EQ(a, b)
#define EXPECT_CM_EQUAL(a, b) EXPECT_EQ(a, b)

} // namespace genie::contact

#endif // GENIE_CONTACT_TEST_HELPERS_H
