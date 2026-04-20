/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_BACKEND_TEST_UTILS_H
#define GENIE_BACKEND_TEST_UTILS_H

#include <algorithm>
#include <vector>
#include <random>
#include <cmath>
#include <numeric>
#include <type_traits>
#include <iostream>
#include <initializer_list>

#include "backend.h"

#ifdef GENIE_HAS_XTENSOR_BACKEND
#include <xtensor/xtensor.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xmath.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xeval.hpp>
#elif defined(GENIE_HAS_EIGEN_BACKEND)
#include <Eigen/Dense>
#endif

namespace genie::backend::test_utils {

// --- Printing Helpers ---

template <typename T>
inline void PrintTo(const std::vector<T>& v, std::ostream* os) {
    *os << "[";
    for (size_t idx_i = 0; idx_i < v.size(); ++idx_i) {
        *os << v[idx_i];
        if (idx_i != v.size() - 1) *os << ", ";
    }
    *os << "]";
}

// --- Matrix Creation & Manipulation ---

template <typename T>
inline auto create_matrix(std::initializer_list<std::initializer_list<T>> data) {
#ifdef GENIE_BACKEND_XTENSOR
    return xt::xtensor<T, 2>(data);
#elif defined(GENIE_BACKEND_EIGEN)
    size_t rows = data.size();
    size_t cols = rows > 0 ? data.begin()->size() : 0;
    Eigen::Matrix<T, -1, -1, Eigen::RowMajor> mat(static_cast<Eigen::Index>(rows), static_cast<Eigen::Index>(cols));
    size_t idx_i = 0;
    for (auto row : data) {
        size_t idx_j = 0;
        for (auto val : row) {
            mat(static_cast<Eigen::Index>(idx_i), static_cast<Eigen::Index>(idx_j)) = val;
            idx_j++;
        }
        idx_i++;
    }
    return mat;
#else
    std::vector<std::vector<T>> mat;
    for (auto row : data) mat.push_back(row);
    return mat;
#endif
}

template <typename T>
inline auto create_vector(const std::vector<T>& data) {
#ifdef GENIE_BACKEND_XTENSOR
    return xt::xtensor<T, 1>(xt::adapt(data));
#elif defined(GENIE_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, 1> vec(static_cast<Eigen::Index>(data.size()));
    for (size_t idx_i = 0; idx_i < data.size(); ++idx_i) {
        vec(static_cast<Eigen::Index>(idx_i)) = data[idx_i];
    }
    return vec;
#else
    return data;
#endif
}

template <typename T>
inline auto create_vector(std::initializer_list<T> data) {
#ifdef GENIE_BACKEND_XTENSOR
    return xt::xtensor<T, 1>(data);
#elif defined(GENIE_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, 1> vec(static_cast<Eigen::Index>(data.size()));
    size_t idx_i = 0;
    for (auto val : data) {
        vec(static_cast<Eigen::Index>(idx_i++)) = val;
    }
    return vec;
#else
    return std::vector<T>(data);
#endif
}

// --- Common Operations ---

template <typename T1, typename T2>
inline bool equal(const T1& a, const T2& b) {
    if constexpr (std::is_scalar_v<T1> || std::is_scalar_v<T2>) {
        return a == b;
    } else {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        if constexpr (detail::has_shape<T1>::value && detail::has_shape<T2>::value) {
            if (a.shape() != b.shape()) return false;
            return xt::all(xt::equal(a, b));
        } else
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
        if constexpr (detail::has_rows<T1>::value && detail::has_rows<T2>::value) {
            if (a.size() != b.size()) return false;
            return (a.array() == b.array()).all();
        } else
#endif
        {
            return a == b;
        }
    }
}

template <typename T1, typename T2>
inline auto not_equal(const T1& a, const T2& b) {
    if constexpr (std::is_scalar_v<T1> || std::is_scalar_v<T2>) {
        return a != b;
    } else {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        if constexpr (detail::has_shape<T1>::value && detail::has_shape<T2>::value) {
            return xt::eval(xt::not_equal(a, b));
        } else
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
        if constexpr (detail::has_rows<T1>::value && detail::has_rows<T2>::value) {
            return (a.array() != b.array()).matrix();
        } else
#endif
        {
            if constexpr (std::is_same_v<T1, T2> && !std::is_scalar_v<typename T1::value_type>) {
                 std::vector<bool> result;
                 result.reserve(a.size());
                 for (size_t i = 0; i < a.size(); ++i) {
                     result.push_back(a[i] != b[i]);
                 }
                 return result;
            } else {
                return a != b;
            }
        }
    }
}

template <typename T>
inline auto get(const T& data, size_t i) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<T>::value) {
        return data(i);
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<T>::value) {
        return data(static_cast<Eigen::Index>(i));
    } else 
#endif
    {
        return data[i];
    }
}

template <typename T>
inline auto get(const T& data, size_t i, size_t j) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<T>::value) {
        return data(i, j);
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<T>::value) {
        return data(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j));
    } else 
#endif
    {
        return data[i][j];
    }
}

template <typename T>
inline size_t rows(const T& data) {
    return genie::backend::get_mat_shape(data, 0);
}

template <typename T>
inline size_t cols(const T& data) {
    return genie::backend::get_mat_shape(data, 1);
}

template <typename T>
inline auto sum(const T& data) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<T>::value) {
        return xt::sum(data)();
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<T>::value) {
        return data.template cast<uint64_t>().sum();
    } else 
#endif
    {
        uint64_t s = 0;
        if constexpr (std::is_scalar_v<typename T::value_type>) {
            for (const auto& v : data) s += v;
        } else {
            for (const auto& row : data) for (const auto& val : row) s += val;
        }
        return s;
    }
}

template <typename T>
inline auto amax(const T& mat) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<T>::value) {
        return xt::amax(mat)();
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<T>::value) {
        return mat.maxCoeff();
    } else 
#endif
    {
        if constexpr (std::is_scalar_v<typename T::value_type>) {
            return *std::max_element(mat.begin(), mat.end());
        } else {
            typename T::value_type::value_type m = mat[0][0];
            for (const auto& row : mat) for (const auto& val : row) if (val > m) m = val;
            return m;
        }
    }
}

template <typename T>
inline auto amin(const T& mat) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<T>::value) {
        return xt::amin(mat)();
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<T>::value) {
        return mat.minCoeff();
    } else 
#endif
    {
        if constexpr (std::is_scalar_v<typename T::value_type>) {
            return *std::min_element(mat.begin(), mat.end());
        } else {
            typename T::value_type::value_type m = mat[0][0];
            for (const auto& row : mat) for (const auto& val : row) if (val < m) m = val;
            return m;
        }
    }
}

template <typename E, typename M, typename V>
inline void set_by_mask(E& e, const M& mask, V value) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<E>::value && detail::has_shape<M>::value) {
        xt::filter(e, mask) = value;
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<E>::value && detail::has_rows<M>::value) {
        for (int i = 0; i < e.rows(); ++i) {
            for (int j = 0; j < e.cols(); ++j) {
                if (mask(i, j)) e(i, j) = static_cast<typename E::Scalar>(value);
            }
        }
    } else 
#endif
    {
        for (size_t i = 0; i < e.size(); ++i) {
            for (size_t j = 0; j < e[i].size(); ++j) {
                if (mask[i][j]) e[i][j] = static_cast<typename E::value_type::value_type>(value);
            }
        }
    }
}

template <typename T>
inline auto argsort(const T& v) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<T>::value) {
        return xt::eval(xt::argsort(v));
    } else 
#endif
    {
        std::vector<uint32_t> indices(v.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::stable_sort(indices.begin(), indices.end(),
                  [&v](uint32_t i1, uint32_t i2) { 
#ifdef GENIE_HAS_EIGEN_BACKEND
                      if constexpr (detail::has_rows<T>::value) {
                          return v(static_cast<Eigen::Index>(i1)) < v(static_cast<Eigen::Index>(i2)); 
                      } else
#endif
                      {
                          return v[i1] < v[i2]; 
                      }
                  });
        return indices;
    }
}

// --- Random & Sequence Generation ---

template <typename T>
inline auto linspace(T min_val, T max_val, size_t n) {
#ifdef GENIE_BACKEND_XTENSOR
    return xt::xtensor<T, 1>(xt::linspace<T>(min_val, max_val, n));
#elif defined(GENIE_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, 1> vec(static_cast<Eigen::Index>(n));
    if (n == 0) return vec;
    if (n == 1) { vec(0) = min_val; return vec; }
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
    if (n == 1) { vec[0] = min_val; return vec; }
    double start = static_cast<double>(min_val);
    double end = static_cast<double>(max_val);
    double step = (end - start) / (static_cast<double>(n) - 1.0);
    for (size_t i = 0; i < n; ++i) {
        vec[i] = static_cast<T>(std::round(start + static_cast<double>(i) * step));
    }
    return vec;
#endif
}

template <typename T>
inline auto permutation(T n) {
    std::vector<T> p(static_cast<size_t>(n));
    std::iota(p.begin(), p.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(p.begin(), p.end(), g);
    return p;
}

template <typename T>
inline auto create_zeros(size_t rows, size_t cols) {
#ifdef GENIE_BACKEND_XTENSOR
    return xt::xtensor<T, 2>(xt::zeros<T>({rows, cols}));
#elif defined(GENIE_BACKEND_EIGEN)
    return Eigen::Matrix<T, -1, -1, Eigen::RowMajor>::Zero(static_cast<Eigen::Index>(rows), static_cast<Eigen::Index>(cols));
#else
    return std::vector<std::vector<T>>(rows, std::vector<T>(cols, static_cast<T>(0)));
#endif
}

template <typename T>
inline auto create_zeros(size_t n) {
#ifdef GENIE_BACKEND_XTENSOR
    return xt::xtensor<T, 1>(xt::zeros<T>({n}));
#elif defined(GENIE_BACKEND_EIGEN)
    return Eigen::Matrix<T, -1, 1>::Zero(static_cast<Eigen::Index>(n));
#else
    return std::vector<T>(n, static_cast<T>(0));
#endif
}

template <typename T>
inline auto create_ones(size_t n) {
#ifdef GENIE_BACKEND_XTENSOR
    return xt::xtensor<T, 1>(xt::ones<T>({n}));
#elif defined(GENIE_BACKEND_EIGEN)
    return Eigen::Matrix<T, -1, 1>::Ones(static_cast<Eigen::Index>(n));
#else
    return std::vector<T>(n, static_cast<T>(1));
#endif
}

template <typename T, typename S>
inline auto add(const T& a, S b) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<T>::value) {
        return xt::eval(a + b);
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<T>::value) {
        return (a.array() + static_cast<typename T::Scalar>(b)).matrix();
    } else 
#endif
    {
        auto res = a;
        for (auto&& v : res) v += b;
        return res;
    }
}

template <typename T, typename S>
inline auto subtract(const T& a, S b) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<T>::value) {
        return xt::eval(a - b);
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<T>::value) {
        return (a.array() - static_cast<typename T::Scalar>(b)).matrix();
    } else 
#endif
    {
        auto res = a;
        for (auto&& v : res) v -= b;
        return res;
    }
}

template <typename T, typename S>
inline auto divide(const T& a, S b) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<T>::value) {
        return xt::eval(a / b);
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<T>::value) {
        return (a.array() / static_cast<typename T::Scalar>(b)).matrix();
    } else 
#endif
    {
        auto res = a;
        for (auto&& v : res) v /= b;
        return res;
    }
}

template <typename Target, typename Source>
inline auto cast(const Source& data) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<Source>::value) {
        return xt::eval(xt::cast<Target>(data));
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<Source>::value) {
        return data.template cast<Target>();
    } else 
#endif
    {
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
}

template <typename T>
inline auto sort(const T& data) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<T>::value) {
        return xt::eval(xt::sort(data));
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<T>::value) {
        auto sorted = data;
        std::sort(sorted.data(), sorted.data() + sorted.size());
        return sorted;
    } else 
#endif
    {
        auto sorted = data;
        std::sort(sorted.begin(), sorted.end());
        return sorted;
    }
}

template <typename T>
inline bool all(const T& data) {
    if constexpr (std::is_scalar_v<T>) {
        return static_cast<bool>(data);
    } else {
#ifdef GENIE_HAS_XTENSOR_BACKEND
        if constexpr (detail::has_shape<T>::value) {
            return xt::all(data)();
        } else
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
        if constexpr (detail::has_rows<T>::value) {
            return data.array().all();
        } else
#endif
        {
            for (const auto& v : data) {
                if constexpr (std::is_scalar_v<std::decay_t<decltype(v)>>) {
                    if (!v) return false;
                } else {
                    if (!all(v)) return false;
                }
            }
            return true;
        }
    }
}

template <typename T>
inline size_t dims(const T& data) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<T>::value) {
        return data.dimension();
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<T>::value) {
        return static_cast<size_t>(T::MaxColsAtCompileTime == 1 ? 1 : 2);
    } else 
#endif
    {
        if constexpr (std::is_scalar_v<typename T::value_type>) return 1;
        return 2;
    }
}

template <typename T, typename S>
inline void add_inplace(T& a, S b) {
#ifdef GENIE_HAS_XTENSOR_BACKEND
    if constexpr (detail::has_shape<T>::value) {
        a += b;
    } else 
#endif
#ifdef GENIE_HAS_EIGEN_BACKEND
    if constexpr (detail::has_rows<T>::value) {
        a.array() += static_cast<typename T::Scalar>(b);
    } else 
#endif
    {
        for (auto&& v : a) v += b;
    }
}

template <typename T>
inline auto random_randint_vec(size_t n, T min_val, T max_val) {
#ifdef GENIE_BACKEND_XTENSOR
    using UT = std::decay_t<T>;
    if constexpr (std::is_same_v<UT, bool>) {
        return xt::xtensor<bool, 1>(xt::random::randint<int>({n}, static_cast<int>(min_val), static_cast<int>(max_val)));
    } else {
        return xt::xtensor<T, 1>(xt::random::randint<T>({n}, min_val, max_val));
    }
#elif defined(GENIE_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, 1> vec(static_cast<Eigen::Index>(n));
    std::random_device rd;
    std::mt19937 gen(rd());
    using UT = std::decay_t<T>;
    using DistType = std::conditional_t<std::is_same_v<UT, bool>, int, UT>;
    std::uniform_int_distribution<DistType> dis(static_cast<DistType>(min_val), static_cast<DistType>(max_val - 1));
    for (int i = 0; i < vec.size(); ++i) vec(i) = static_cast<T>(dis(gen));
    return vec;
#else
    std::vector<T> vec(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    using UT = std::decay_t<T>;
    using DistType = std::conditional_t<std::is_same_v<UT, bool>, int, UT>;
    std::uniform_int_distribution<DistType> dis(static_cast<DistType>(min_val), static_cast<DistType>(max_val - 1));
    for (auto&& v : vec) v = static_cast<T>(dis(gen));
    return vec;
#endif
}

template <typename T>
inline auto random_randint_mat(size_t rows, size_t cols, T min_val, T max_val) {
#ifdef GENIE_BACKEND_XTENSOR
    using UT = std::decay_t<T>;
    if constexpr (std::is_same_v<UT, bool>) {
        return xt::xtensor<bool, 2>(xt::random::randint<int>({rows, cols}, static_cast<int>(min_val), static_cast<int>(max_val)));
    } else {
        return xt::xtensor<T, 2>(xt::random::randint<T>({rows, cols}, min_val, max_val));
    }
#elif defined(GENIE_BACKEND_EIGEN)
    Eigen::Matrix<T, -1, -1, Eigen::RowMajor> mat(static_cast<Eigen::Index>(rows), static_cast<Eigen::Index>(cols));
    std::random_device rd;
    std::mt19937 gen(rd());
    using UT = std::decay_t<T>;
    using DistType = std::conditional_t<std::is_same_v<UT, bool>, int, UT>;
    std::uniform_int_distribution<DistType> dis(static_cast<DistType>(min_val), static_cast<DistType>(max_val - 1));
    for (int i = 0; i < mat.rows(); ++i) {
        for (int j = 0; j < mat.cols(); ++j) {
            mat(i, j) = static_cast<T>(dis(gen));
        }
    }
    return mat;
#else
     std::vector<std::vector<T>> mat(rows, std::vector<T>(cols));
     std::random_device rd;
     std::mt19937 gen(rd());
     using UT = std::decay_t<T>;
     using DistType = std::conditional_t<std::is_same_v<UT, bool>, int, UT>;
     std::uniform_int_distribution<DistType> dis(static_cast<DistType>(min_val), static_cast<DistType>(max_val - 1));
     for (auto&& row : mat) for (auto&& v : row) v = static_cast<T>(dis(gen));
     return mat;
#endif
}

template <typename T>
inline auto random_matrix(size_t rows, size_t cols, T min_val, T max_val) {
    return random_randint_mat<T>(rows, cols, min_val, max_val);
}

} // namespace genie::backend::test_utils

#endif // GENIE_BACKEND_TEST_UTILS_H
