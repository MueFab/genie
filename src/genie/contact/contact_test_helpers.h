/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CONTACT_CONTACT_TEST_HELPERS_H_
#define SRC_GENIE_CONTACT_CONTACT_TEST_HELPERS_H_

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <numeric>
#include <random>
#include <type_traits>
#include <vector>

#include "genie/contact/contact_types.h"

#ifdef GENIE_CONTACT_BACKEND_XTENSOR
#include <xtensor/xsort.hpp>
#include <xtensor/xmath.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xadapt.hpp>
#endif

namespace std {
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << v[i];
        if (i != v.size() - 1) os << ", ";
    }
    os << "]";
    return os;
}
}  // namespace std

// Generalized arithmetic operators for std::vector
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

#ifdef GENIE_CONTACT_BACKEND_XTENSOR
template <typename T>
inline auto to_xexpr(const T& data) {
    if constexpr (xt::is_xexpression<T>::value) {
        return data;
    } else {
        return xt::adapt(data);
    }
}
#endif

template <typename T>
inline void PrintTo(const std::vector<T>& v, std::ostream* os) {
    *os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        *os << v[i];
        if (i != v.size() - 1) *os << ", ";
    }
    *os << "]";
}

template <typename T>
inline auto create_vector(const std::vector<T>& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    xt::xtensor<T, 1> res = xt::adapt(data);
    return res;
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
inline auto create_vector(std::initializer_list<T> data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::xtensor<T, 1>(data);
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
    return xt::xtensor<bool, 1>(xt::adapt(data));
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

struct is_std_vector : std::false_type {};
template <typename T, typename A>
struct is_std_vector<std::vector<T, A>> : std::true_type {};

template <typename Container>
auto get_elem(const Container& c, size_t i) {
    if constexpr (is_std_vector<Container>::value) {
        return c[i];
    } else {
        // Assume Eigen or xtensor
        return c(i);
    }
}

template <typename T>
inline auto create_matrix(std::initializer_list<std::initializer_list<T>> data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::xtensor<T, 2>(data);
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
inline auto sum(const T& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    auto expr = to_xexpr(data);
    return xt::sum(expr)(0);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return data.template cast<uint64_t>().sum();
#else
    uint64_t s = 0;
    for (const auto& row : data) {
        if constexpr (std::is_scalar_v<std::decay_t<decltype(row)>>) {
            s += row;
        } else {
             for (const auto& v : row) s += v;
        }
    }
    return s;
#endif
}

template <typename Target, typename Source>
inline auto cast(const Source& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::cast<Target>(data);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return data.template cast<Target>();
#else
    if constexpr (std::is_scalar_v<std::decay_t<Source>>) {
        return static_cast<Target>(data);
    } else {
        using InnerType = typename Source::value_type;
        if constexpr (std::is_scalar_v<InnerType>) {
            std::vector<Target> result;
            result.reserve(data.size());
            for (const auto& val : data) result.push_back(static_cast<Target>(val));
            return result;
        } else {
            std::vector<std::vector<Target>> result;
            result.reserve(data.size());
            for (const auto& row : data) {
                std::vector<Target> inner;
                inner.reserve(row.size());
                for (const auto& val : row) inner.push_back(static_cast<Target>(val));
                result.push_back(inner);
            }
            return result;
        }
    }
#endif
}

template <typename T>
inline auto sort(const T& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    auto expr = to_xexpr(data);
    return xt::sort(expr);
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
inline bool equal(const T1& a, const T2& b) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    if constexpr (std::is_scalar_v<T1> || std::is_scalar_v<T2>) {
        return a == b;
    } else {
        // xt::equal throws broadcast_error on shape mismatch — check shape first
        auto xa = to_xexpr(a);
        auto xb = to_xexpr(b);
        if (xa.shape() != xb.shape()) return false;
        return xt::all(xt::equal(xa, xb));
    }
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    if constexpr (std::is_scalar_v<T1> || std::is_scalar_v<T2>) {
        return a == b;
    } else {
        if (a.size() != b.size()) return false;
        // Check if it's an Eigen type by verifying if it's not a std::vector
        if constexpr (!std::is_same_v<std::decay_t<T1>, std::vector<typename std::decay_t<T1>::value_type>>) {
            return (a.array() == b.array()).all();
        } else {
            for (size_t i = 0; i < a.size(); ++i) {
                if (get_elem(a, i) != get_elem(b, i)) return false;
            }
            return true;
        }
    }
#else
    if constexpr (std::is_scalar_v<T1> || std::is_scalar_v<T2>) {
        return a == b;
    } else {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (get_elem(a, i) != get_elem(b, i)) return false;
        }
        return true;
    }
#endif
}

template <typename T>
inline bool all(const T& data) {
    if constexpr (std::is_scalar_v<T>) {
        return data;
    } else {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
        return xt::all(to_xexpr(data));
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
        return data.array().all();
#else
        for (const auto& v : data) if (!all(v)) return false;
        return true;
#endif
    }
}

template <typename T>
inline size_t dims(const T& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return data.dimension();
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return static_cast<size_t>(T::MaxColsAtCompileTime == 1 ? 1 : 2);
#else
    if constexpr (std::is_scalar_v<T>) return 0;
    using InnerType = typename T::value_type;
    if constexpr (std::is_scalar_v<InnerType>) return 1;
    return 2;
#endif
}

// Specialization for vector<vector<T>> to get dims correct if T is not nested
template <typename T>
size_t dims(const std::vector<std::vector<T>>&) {
    return 2;
}

template <typename T, typename S>
inline void add_inplace(T& a, S b) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    a += b;
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    a.array() += static_cast<typename T::Scalar>(b);
#else
    for (auto& v : a) v += b;
#endif
}

template <typename T1, typename T2>
inline auto not_equal(const T1& a, const T2& b) {
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
inline auto get(const T& data, size_t i) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return data(i);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return data(static_cast<Eigen::Index>(i));
#else
    return data[i];
#endif
}

template <typename T>
inline auto get(const T& data, size_t i, size_t j) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return data(i, j);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return data(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j));
#else
    return data[i][j];
#endif
}

template <typename T>
inline size_t rows(const T& data) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return data.shape(0);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return static_cast<size_t>(data.rows());
#else
    return data.size();
#endif
}

template <typename T>
inline size_t cols(const T& data) {
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
inline auto random_randint_vec(size_t n, T min_val, T max_val) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::xtensor<T, 1>(xt::random::randint<T>({n}, min_val, max_val));
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
inline auto random_randint_mat(size_t rows, size_t cols, T min_val, T max_val) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::xtensor<T, 2>(xt::random::randint<T>({rows, cols}, min_val, max_val));
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
inline auto create_zeros(size_t rows, size_t cols) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::xtensor<T, 2>(xt::zeros<T>({rows, cols}));
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, -1, Eigen::RowMajor> mat = Eigen::Matrix<T, -1, -1, Eigen::RowMajor>::Zero(static_cast<Eigen::Index>(rows), static_cast<Eigen::Index>(cols));
    return mat;
#else
    return std::vector<std::vector<T>>(rows, std::vector<T>(cols, static_cast<T>(0)));
#endif
}

template <typename T, typename S>
inline auto subtract(const T& a, S b) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    xt::xtensor<uint64_t, 1> res = a - b;
    return res;
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return (a.array() - static_cast<typename T::Scalar>(b)).matrix();
#else
    auto res = a;
    for (auto& v : res) v -= b;
    return res;
#endif
}

template <typename T, typename S>
inline auto divide(const T& a, S b) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    xt::xtensor<uint64_t, 1> res = a / b;
    return res;
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return (a.array() / static_cast<typename T::Scalar>(b)).matrix();
#else
    auto res = a;
    for (auto& v : res) v /= b;
    return res;
#endif
}

template <typename T>
inline auto create_zeros(size_t n) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::xtensor<T, 1>(xt::zeros<T>({n}));
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, 1> vec = Eigen::Matrix<T, -1, 1>::Zero(static_cast<Eigen::Index>(n));
    return vec;
#else
    return std::vector<T>(n, static_cast<T>(0));
#endif
}

template <typename T>
inline auto create_ones(size_t n) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::xtensor<T, 1>(xt::ones<T>({n}));
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, 1> vec = Eigen::Matrix<T, -1, 1>::Ones(static_cast<Eigen::Index>(n));
    return vec;
#else
    return std::vector<T>(n, static_cast<T>(1));
#endif
}

template <typename T, typename S>
inline auto add(const T& a, S b) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    xt::xtensor<uint64_t, 1> res = a + b;
    return res;
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    return (a.array() + static_cast<typename T::Scalar>(b)).matrix();
#else
    auto res = a;
    for (auto& v : res) v += b;
    return res;
#endif
}

template <typename T>
inline auto linspace(T min_val, T max_val, size_t n) {
#ifdef GENIE_CONTACT_BACKEND_XTENSOR
    return xt::xtensor<T, 1>(xt::linspace<T>(min_val, max_val, n));
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, 1> vec(static_cast<Eigen::Index>(n));
    if (n == 0) return vec;
    if (n == 1) {
        vec(0) = min_val;
        return vec;
    }
    double start = static_cast<double>(min_val);
    double end = static_cast<double>(max_val);
    double step = (end - start) / (static_cast<double>(n) - 1.0);
    for (size_t i = 0; i < n; ++i) {
        vec(static_cast<Eigen::Index>(i)) = static_cast<T>(std::round(start + static_cast<double>(i) * step));
    }
    return vec;
#else
    std::vector<T> vec(n);
    if (n == 0) return vec;
    if (n == 1) {
        vec[0] = min_val;
        return vec;
    }
    double start = static_cast<double>(min_val);
    double end = static_cast<double>(max_val);
    double step = (end - start) / (static_cast<double>(n) - 1.0);
    for (size_t i = 0; i < n; ++i) {
        vec[i] = static_cast<T>(std::round(start + static_cast<double>(i) * step));
    }
    return vec;
#endif
}

#define ASSERT_CM_EQUAL(a, b) ASSERT_TRUE(genie::contact::equal(a, b))
#define EXPECT_CM_EQUAL(a, b) EXPECT_TRUE(genie::contact::equal(a, b))

}  // namespace genie::contact

#endif  // SRC_GENIE_CONTACT_CONTACT_TEST_HELPERS_H_
