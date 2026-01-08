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

#ifdef GENIE_CONTACT_BACKEND_XTENSOR
#include <xtensor/xsort.hpp>
#include <xtensor/xmath.hpp>
#include <xtensor/xcast.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xadapt.hpp>
#endif

namespace genie::contact {

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

template <typename T>
auto create_vector(const std::vector<T>& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::adapt(data);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, 1> vec(static_cast<Eigen::Index>(data.size()));
    for (size_t i = 0; i < data.size(); ++i) {
        vec(static_cast<Eigen::Index>(i)) = data[i];
    }
    return vec;
#else
    return data;
#endif
}

// Support for initializer_list
template <typename T>
auto create_vector(std::initializer_list<T> data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::xarray<T>(data);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, 1> vec(static_cast<Eigen::Index>(data.size()));
    size_t i = 0;
    for (auto val : data) {
        vec(static_cast<Eigen::Index>(i++)) = val;
    }
    return vec;
#else
    return std::vector<T>(data);
#endif
}

// Specialization for std::vector<bool>
template <>
inline auto create_vector<bool>(const std::vector<bool>& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::adapt(data);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    Eigen::Matrix<bool, -1, 1> vec(static_cast<Eigen::Index>(data.size()));
    for (size_t i = 0; i < data.size(); ++i) {
        vec(static_cast<Eigen::Index>(i)) = static_cast<bool>(data[i]);
    }
    return vec;
#else
    return data;
#endif
}

template <typename T>
auto create_matrix(std::initializer_list<std::initializer_list<T>> data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::xarray<T>(data);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    size_t rows = data.size();
    size_t cols = rows > 0 ? data.begin()->size() : 0;
    Eigen::Matrix<T, -1, -1, Eigen::RowMajor> mat(static_cast<Eigen::Index>(rows), static_cast<Eigen::Index>(cols));
    size_t i = 0;
    for (auto row : data) {
        size_t j = 0;
        for (auto val : row) {
            mat(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j)) = val;
            j++;
        }
        i++;
    }
    return mat;
#else
    std::vector<std::vector<T>> mat;
    for (auto row : data) mat.push_back(row);
    return mat;
#endif
}

template <typename T>
auto sum(const T& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::sum(data)(0);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return data.template cast<uint64_t>().sum();
#else
    return std::accumulate(data.begin(), data.end(), 0ULL);
#endif
}

template <typename Target, typename Source>
auto cast(const Source& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::cast<Target>(data);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return data.template cast<Target>();
#else
    std::vector<Target> result;
    result.reserve(data.size());
    for (const auto& val : data) {
        result.push_back(static_cast<Target>(val));
    }
    return result;
#endif
}

template <typename T>
auto sort(const T& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::sort(data);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    auto sorted = data;
    std::sort(sorted.data(), sorted.data() + sorted.size());
    return sorted;
#else
    auto sorted = data;
    std::sort(sorted.begin(), sorted.end());
    return sorted;
#endif
}

template <typename T1, typename T2>
bool equal(const T1& a, const T2& b) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::all(xt::equal(a, b));
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    if (a.size() != b.size()) return false;
    return (a.array() == b.array()).all();
#else
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
#endif
}

template <typename T>
bool all(const T& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::all(data);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return data.array().all();
#else
    for (const auto& v : data) if (!v) return false;
    return true;
#endif
}

inline bool all(bool data) {
    return data;
}

template <typename T>
size_t dims(const T& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return data.dimension();
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return static_cast<size_t>(T::MaxColsAtCompileTime == 1 ? 1 : 2);
#else
    // For std::vector or std::vector<std::vector>>
    // This is hard to detect generically for all depths but we only care about 1 and 2.
    return 1; // Default to 1 for simplicity, can specialize if needed
#endif
}

template <typename T, typename S>
void add_inplace(T& a, S b) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    a += b;
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    a.array() += static_cast<typename T::Scalar>(b);
#else
    for (auto& v : a) v += b;
#endif
}

template <typename T1, typename T2>
auto not_equal(const T1& a, const T2& b) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::not_equal(a, b);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return (a.array() != b.array()).matrix();
#else
    std::vector<bool> result;
    result.reserve(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result.push_back(a[i] != b[i]);
    }
    return result;
#endif
}

template <typename T>
auto get(const T& data, size_t i) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return data(i);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return data(static_cast<Eigen::Index>(i));
#else
    return data[i];
#endif
}

template <typename T>
auto get(const T& data, size_t i, size_t j) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return data(i, j);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return data(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j));
#else
    return data[i][j];
#endif
}

template <typename T>
size_t rows(const T& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return data.shape(0);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return static_cast<size_t>(data.rows());
#else
    return data.size();
#endif
}

template <typename T>
size_t cols(const T& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return data.shape(1);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return static_cast<size_t>(data.cols());
#else
    if (data.empty()) return 0;
    return data[0].size();
#endif
}

template <typename T>
auto random_randint_vec(size_t n, T min_val, T max_val) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::random::randint<T>({n}, min_val, max_val);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, 1> vec(static_cast<Eigen::Index>(n));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<T> dis(min_val, max_val - 1);
    for (int i = 0; i < vec.size(); ++i) vec(i) = dis(gen);
    return vec;
#else
    std::vector<T> vec(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<T> dis(min_val, max_val - 1);
    for (auto& v : vec) v = dis(gen);
    return vec;
#endif
}

template <typename T>
auto random_randint_mat(size_t rows, size_t cols, T min_val, T max_val) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::random::randint<T>({rows, cols}, min_val, max_val);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, -1, Eigen::RowMajor> mat(static_cast<Eigen::Index>(rows), static_cast<Eigen::Index>(cols));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<T> dis(min_val, max_val - 1);
    for (int i = 0; i < mat.rows(); ++i) {
        for (int j = 0; j < mat.cols(); ++j) {
            mat(i, j) = dis(gen);
        }
    }
    return mat;
#else
     std::vector<std::vector<T>> mat(rows, std::vector<T>(cols));
     std::random_device rd;
     std::mt19937 gen(rd());
     std::uniform_int_distribution<T> dis(min_val, max_val - 1);
     for (auto& row : mat) for (auto& v : row) v = dis(gen);
     return mat;
#endif
}

template <typename T>
auto create_zeros(size_t rows, size_t cols) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::zeros<T>({rows, cols});
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, -1, Eigen::RowMajor> mat = Eigen::Matrix<T, -1, -1, Eigen::RowMajor>::Zero(static_cast<Eigen::Index>(rows), static_cast<Eigen::Index>(cols));
    return mat;
#else
    return std::vector<std::vector<T>>(rows, std::vector<T>(cols, static_cast<T>(0)));
#endif
}

template <typename T, typename S>
auto subtract(const T& a, S b) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return a - b;
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return (a.array() - static_cast<typename T::Scalar>(b)).matrix();
#else
    auto res = a;
    for (auto& v : res) v -= b;
    return res;
#endif
}

template <typename T, typename S>
auto divide(const T& a, S b) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return a / b;
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return (a.array() / static_cast<typename T::Scalar>(b)).matrix();
#else
    auto res = a;
    for (auto& v : res) v /= b;
    return res;
#endif
}

template <typename T>
auto create_zeros(size_t n) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::zeros<T>({n});
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, 1> vec = Eigen::Matrix<T, -1, 1>::Zero(static_cast<Eigen::Index>(n));
    return vec;
#else
    return std::vector<T>(n, static_cast<T>(0));
#endif
}

template <typename T>
auto create_ones(size_t n) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::ones<T>({n});
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, 1> vec = Eigen::Matrix<T, -1, 1>::Ones(static_cast<Eigen::Index>(n));
    return vec;
#else
    return std::vector<T>(n, static_cast<T>(1));
#endif
}

template <typename T, typename S>
auto add(const T& a, S b) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return a + b;
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return (a.array() + static_cast<typename T::Scalar>(b)).matrix();
#else
    auto res = a;
    for (auto& v : res) v += b;
    return res;
#endif
}

#define ASSERT_CM_EQUAL(a, b) ASSERT_TRUE(genie::contact::equal(a, b))
#define EXPECT_CM_EQUAL(a, b) EXPECT_TRUE(genie::contact::equal(a, b))

} // namespace genie::contact

#endif // GENIE_CONTACT_TEST_HELPERS_H
