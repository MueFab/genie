/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_NDARRAY_H_
#define SRC_GENIE_CORE_NDARRAY_H_

// -----------------------------------------------------------------------------

#include <algorithm>
#include <cstddef>
#include <functional>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

/**
 * @brief Multi-dimensional array class.
 *
 * This class provides a flexible and efficient way to handle multi-dimensional arrays
 * with dynamic sizing and element access.
 */
template<typename T>
class NDArray {
 private:
    std::vector<size_t> dimensions;  ///< Dimensions of the array.
    std::vector<size_t> strides;     ///< Strides for each dimension.
    std::vector<T> data;           ///< Data storage for the array elements.

    void computeStrides() {
        strides.resize(dimensions.size());
        size_t stride = 1;
        for (int idx_i = static_cast<int>(dimensions.size()) - 1; idx_i >= 0; --idx_i) {
            strides[idx_i] = stride;
            stride *= dimensions[idx_i];
        }
    }

 public:
    /**
     * @brief Default constructor.
     */
    NDArray() : dimensions(), strides(), data() {}

    /**
     * @brief Constructor from dimensions.
     */
    explicit NDArray(const std::vector<size_t>& dims) : dimensions(dims) {
        size_t total = std::accumulate(
            dimensions.begin(), dimensions.end(), 1, std::multiplies<size_t>());
        data.resize(total);
        computeStrides();
    }

    /**
     * @brief Constructor from dimensions and buffer.
     */
    NDArray(const std::vector<size_t>& dims, std::vector<T>&& buffer)
        : dimensions(dims), data(std::move(buffer)) {
        computeStrides();
    }

    /**
     * @brief Constructor from dimensions and a buffer by reference.
     */
    NDArray(const std::vector<size_t>& dims, const std::vector<T>& buffer)
        : dimensions(dims), data(buffer) {
        computeStrides();
    }

    /**
     * @brief Move constructor.
     */
    NDArray(NDArray&& other) noexcept
        : dimensions(std::move(other.dimensions)),
          strides(std::move(other.strides)),
          data(std::move(other.data)) {}

    /**
     * @brief Copy constructor.
     */
    NDArray(const NDArray& other)
        : dimensions(other.dimensions),
          data(other.data) {
        computeStrides();
    }

    /**
     * @brief Move assignment operator.
     */
    NDArray& operator=(NDArray&& other) noexcept {
        if (this != &other) {
            dimensions = std::move(other.dimensions);
            strides = std::move(other.strides);
            data = std::move(other.data);
        }
        return *this;
    }

    /**
     * @brief Copy assignment operator.
     */
    NDArray& operator=(const NDArray& other) {
        if (this != &other) {
            dimensions = other.dimensions;
            data = other.data;
            computeStrides();
        }
        return *this;
    }

    /**
     * @brief Resize the array.
     */
    void resize(const std::vector<size_t>& newDims, const T& defaultValue = T()) {
        dimensions = newDims;
        computeStrides();
        size_t newSize = std::accumulate(
            dimensions.begin(), dimensions.end(), 1, std::multiplies<size_t>());
        data.resize(newSize, defaultValue);
    }

    /**
     * @brief Access element using vector of indices.
     */
    T& operator()(const std::vector<size_t>& indices) {
        if (indices.size() != dimensions.size()) {
            throw std::invalid_argument("Invalid number of indices");
        }
        size_t index = 0;
        for (size_t idx_i = 0; idx_i < indices.size(); ++idx_i) {
            if (indices[idx_i] >= dimensions[idx_i]) {
                throw std::out_of_range("Index out of range");
            }
            index += indices[idx_i] * strides[idx_i];
        }
        return data[index];
    }

    /**
     * @brief Variadic template version for convenient access.
     */
    template<typename... Args>
    T& operator()(Args... args) {
        std::vector<size_t> indices{static_cast<size_t>(args)...};
        return (*this)(indices);
    }

    /**
     * @brief Const version of access element using vector of indices.
     */
    const T& operator()(const std::vector<size_t>& indices) const {
        return const_cast<NDArray*>(this)->operator()(indices);
    }

    /**
     * @brief Const variadic template version for convenient access.
     */
    template<typename... Args>
    const T& operator()(Args... args) const {
        return const_cast<NDArray*>(this)->operator()(args...);
    }

    /**
     * @brief Get the shape of the array.
     */
    const std::vector<size_t>& shape() const { return dimensions; }

    /**
     * @brief Get the total number of elements in the array.
     */
    size_t size() const { return data.size(); }

    /**
     * @brief Fill the array with a specific value.
     */
    void fill(const T& value) { std::fill(data.begin(), data.end(), value); }

    /**
     * @brief Flatten the array.
     */
    NDArray<T> flatten() const {
        return NDArray<T>({data.size()}, std::vector<T>(data));
    }

    template<typename U> friend NDArray<U> operator+(const NDArray<U>& lhs, const NDArray<U>& rhs);
    template<typename U> friend NDArray<U> operator-(const NDArray<U>& lhs, const NDArray<U>& rhs);
    template<typename U> friend NDArray<U> operator*(const NDArray<U>& lhs, const NDArray<U>& rhs);
    template<typename U> friend NDArray<U> operator/(const NDArray<U>& lhs, const NDArray<U>& rhs);
};

// -----------------------------------------------------------------------------

template<typename T>
NDArray<T> operator+(const NDArray<T>& lhs, const NDArray<T>& rhs) {
    if (lhs.shape() != rhs.shape()) throw std::invalid_argument("Shapes do not match");
    NDArray<T> result(lhs.shape());
    std::transform(lhs.data.begin(), lhs.data.end(), rhs.data.begin(), result.data.begin(), std::plus<T>());
    return result;
}

template<typename T>
NDArray<T> operator-(const NDArray<T>& lhs, const NDArray<T>& rhs) {
    if (lhs.shape() != rhs.shape()) throw std::invalid_argument("Shapes do not match");
    NDArray<T> result(lhs.shape());
    std::transform(lhs.data.begin(), lhs.data.end(), rhs.data.begin(), result.data.begin(), std::minus<T>());
    return result;
}

template<typename T>
NDArray<T> operator*(const NDArray<T>& lhs, const NDArray<T>& rhs) {
    if (lhs.shape() != rhs.shape()) throw std::invalid_argument("Shapes do not match");
    NDArray<T> result(lhs.shape());
    std::transform(lhs.data.begin(), lhs.data.end(), rhs.data.begin(), result.data.begin(), std::multiplies<T>());
    return result;
}

template<typename T>
NDArray<T> operator/(const NDArray<T>& lhs, const NDArray<T>& rhs) {
    if (lhs.shape() != rhs.shape()) throw std::invalid_argument("Shapes do not match");
    NDArray<T> result(lhs.shape());
    std::transform(lhs.data.begin(), lhs.data.end(), rhs.data.begin(), result.data.begin(), std::divides<T>());
    return result;
}

// -----------------------------------------------------------------------------

template<typename T>
class NDArrayFactory {
 public:
    static NDArray<T> create(const std::vector<size_t>& dimensions) {
        size_t total = std::accumulate(dimensions.begin(), dimensions.end(), 1, std::multiplies<size_t>());
        return NDArray<T>(dimensions, std::vector<T>(total));
    }

    static NDArray<T> create(const std::vector<size_t>& dimensions, const T& initValue) {
        size_t total = std::accumulate(dimensions.begin(), dimensions.end(), 1, std::multiplies<size_t>());
        return NDArray<T>(dimensions, std::vector<T>(total, initValue));
    }
};

}  // namespace genie::core

#endif  // SRC_GENIE_CORE_NDARRAY_H_
