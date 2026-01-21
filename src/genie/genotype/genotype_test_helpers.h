/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_TEST_HELPERS_H
#define GENIE_GENOTYPE_TEST_HELPERS_H

#include "genotype_coder.h"
#include <algorithm>
#include <vector>
#include <random>
#include <cmath>
#include <numeric>

#ifdef GENIE_GENOTYPE_BACKEND_XTENSOR
#include <xtensor/xtensor.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xmath.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xadapt.hpp>
#elif defined(GENIE_GENOTYPE_BACKEND_EIGEN)
#include <Eigen/Dense>
#endif

namespace genie::genotype {

#ifdef GENIE_GENOTYPE_BACKEND_XTENSOR

template <typename T>
auto random_matrix(size_t nrows, size_t ncols, T min, T max) {
    return xt::eval(xt::cast<T>(xt::random::randint<int32_t>({nrows, ncols}, min, max)));
}

template <typename E1, typename E2>
bool equal(const E1& e1, const E2& e2) {
    return xt::all(xt::equal(e1, e2));
}

template <typename E>
auto sum(E&& e) {
    return xt::sum(std::forward<E>(e))();
}

template <typename E>
auto amax(E&& e) {
    return xt::amax(std::forward<E>(e))();
}

template <typename E>
auto amin(E&& e) {
    return xt::amin(std::forward<E>(e))();
}

template <typename E>
size_t get_nrows(const E& e) {
    return e.shape(0);
}

template <typename E>
size_t get_ncols(const E& e) {
    return e.shape(1);
}

template <typename E, typename V>
void set_element(E& e, size_t i, size_t j, V value) {
    e(i, j) = value;
}

template <typename E, typename M, typename V>
void set_by_mask(E& e, const M& mask, V value) {
    xt::filter(e, mask) = value;
}

template <typename E>
auto argsort(E&& e) {
    return xt::argsort(std::forward<E>(e));
}

template <typename T>
auto create_matrix(std::initializer_list<std::initializer_list<T>> data) {
    return xt::eval(xt::xarray<T>(data));
}

#elif defined(GENIE_GENOTYPE_BACKEND_EIGEN)

template <typename T>
auto random_matrix(size_t nrows, size_t ncols, T min, T max) {
    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> mat(nrows, ncols);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int32_t> dis(static_cast<int32_t>(min), static_cast<int32_t>(max) - 1);
    for (int i = 0; i < (int)nrows; ++i) {
        for (int j = 0; j < (int)ncols; ++j) {
            mat(i, j) = static_cast<T>(dis(gen));
        }
    }
    return mat;
}

template <typename Derived1, typename Derived2>
bool equal(const Eigen::MatrixBase<Derived1>& m1, const Eigen::MatrixBase<Derived2>& m2) {
    return m1 == m2;
}

template <typename Derived>
auto sum(const Eigen::MatrixBase<Derived>& mat) {
    return mat.template cast<uint64_t>().sum();
}

template <typename Derived>
auto amax(const Eigen::MatrixBase<Derived>& mat) {
    return mat.maxCoeff();
}

template <typename Derived>
auto amin(const Eigen::MatrixBase<Derived>& mat) {
    return mat.minCoeff();
}

template <typename Derived>
size_t get_nrows(const Eigen::MatrixBase<Derived>& mat) {
    return static_cast<size_t>(mat.rows());
}

template <typename Derived>
size_t get_ncols(const Eigen::MatrixBase<Derived>& mat) {
    return static_cast<size_t>(mat.cols());
}

template <typename Derived, typename V>
void set_element(Eigen::MatrixBase<Derived>& mat, size_t i, size_t j, V value) {
    const_cast<Eigen::MatrixBase<Derived>&>(mat)(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j)) = static_cast<typename Derived::Scalar>(value);
}

template <typename Derived, typename DerivedM, typename V>
void set_by_mask(Eigen::MatrixBase<Derived>& mat, const Eigen::MatrixBase<DerivedM>& mask, V value) {
    for (int i = 0; i < mat.rows(); ++i) {
        for (int j = 0; j < mat.cols(); ++j) {
            if (mask(i, j)) {
                const_cast<Eigen::MatrixBase<Derived>&>(mat)(i, j) = static_cast<typename Derived::Scalar>(value);
            }
        }
    }
}

template <typename Derived>
auto argsort(const Eigen::MatrixBase<Derived>& v) {
    std::vector<uint32_t> indices(static_cast<size_t>(v.size()));
    std::iota(indices.begin(), indices.end(), 0);
    std::stable_sort(indices.begin(), indices.end(),
              [&v](uint32_t i1, uint32_t i2) { return v(static_cast<Eigen::Index>(i1)) < v(static_cast<Eigen::Index>(i2)); });
    return indices;
}

template <typename T>
auto create_matrix(std::initializer_list<std::initializer_list<T>> data) {
    size_t rows = data.size();
    size_t cols = rows > 0 ? data.begin()->size() : 0;
    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> mat(rows, cols);
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
}

#else // GENIE_GENOTYPE_BACKEND_STD

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
    T m = mat[0][0];
    for (const auto& row : mat) {
        for (const auto& val : row) if (val > m) m = val;
    }
    return m;
}

template <typename T>
auto amin(const std::vector<std::vector<T>>& mat) {
    T m = mat[0][0];
    for (const auto& row : mat) {
        for (const auto& val : row) if (val < m) m = val;
    }
    return m;
}

#endif

} // namespace genie::genotype

#endif // GENIE_GENOTYPE_TEST_HELPERS_H
