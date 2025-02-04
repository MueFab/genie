#include "ndarray.h"
#include <numeric>
#include <algorithm>

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

// Compute strides based on dimensions
template<typename T>
void NDArray<T>::computeStrides() {
    strides.resize(dimensions.size());
    size_t stride = 1;
    for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; --i) {
        strides[i] = stride;
        stride *= dimensions[i];
    }
}

// -----------------------------------------------------------------------------

// Default constructor
template<typename T>
NDArray<T>::NDArray()
    : dimensions(), strides(), data() {}

// -----------------------------------------------------------------------------

// Constructor from dimensions and buffer
template<typename T>
NDArray<T>::NDArray(const std::vector<size_t>& dims, std::vector<T>&& buffer)
    : dimensions(dims), data(std::move(buffer)) {
    computeStrides();
}

// -----------------------------------------------------------------------------

// Constructor from dimensions and a buffer by reference
template<typename T>
NDArray<T>::NDArray(const std::vector<size_t>& dims, const std::vector<T>& buffer)
    : dimensions(dims), data(buffer) {
    computeStrides();
}

// -----------------------------------------------------------------------------

// Move constructor
template<typename T>
NDArray<T>::NDArray(NDArray&& other) noexcept
    : dimensions(std::move(other.dimensions)),
      strides(std::move(other.strides)),
      data(std::move(other.data)) {}

// -----------------------------------------------------------------------------

// Copy constructor
template<typename T>
NDArray<T>::NDArray(const NDArray& other)
    : dimensions(other.dimensions),
      data(other.data) {
    computeStrides();
}

// -----------------------------------------------------------------------------

// Move assignment operator
template<typename T>
NDArray<T>& NDArray<T>::operator=(NDArray&& other) noexcept {
    if (this != &other) {
        dimensions = std::move(other.dimensions);
        strides = std::move(other.strides);
        data = std::move(other.data);
    }
    return *this;
}

// -----------------------------------------------------------------------------

// Copy assignment operator
template<typename T>
NDArray<T>& NDArray<T>::operator=(const NDArray& other) {
    if (this != &other) {
        dimensions = other.dimensions;
        data = other.data;
        computeStrides();
    }
    return *this;
}

// -----------------------------------------------------------------------------

// Resize the array
template<typename T>
void NDArray<T>::resize(const std::vector<size_t>& newDims, const T& defaultValue) {
    dimensions = newDims;
    computeStrides();
    size_t newSize = std::accumulate(
        dimensions.begin(), dimensions.end(), 1, std::multiplies<size_t>());
    data.resize(newSize, defaultValue);
}

// -----------------------------------------------------------------------------

// Access element using vector of indices
template<typename T>
T& NDArray<T>::operator()(const std::vector<size_t>& indices) {
    if (indices.size() != dimensions.size()) {
        throw std::invalid_argument("Invalid number of indices");
    }
    size_t index = 0;
    for (size_t i = 0; i < indices.size(); ++i) {
        if (indices[i] >= dimensions[i]) {
            throw std::out_of_range("Index out of range");
        }
        index += indices[i] * strides[i];
    }
    return data[index];
}

// -----------------------------------------------------------------------------

// Variadic template version for convenient access
template<typename T>
template<typename... Args>
T& NDArray<T>::operator()(Args... args) {
    std::vector<size_t> indices{static_cast<size_t>(args)...};
    return (*this)(indices);
}

// -----------------------------------------------------------------------------

// Const version of access element using vector of indices
template<typename T>
const T& NDArray<T>::operator()(const std::vector<size_t>& indices) const {
    return const_cast<NDArray*>(this)->operator()(indices);
}

// -----------------------------------------------------------------------------

// Const variadic template version for convenient access
template<typename T>
template<typename... Args>
const T& NDArray<T>::operator()(Args... args) const {
    return const_cast<NDArray*>(this)->operator()(args...);
}

// -----------------------------------------------------------------------------

// Get the shape of the array
template<typename T>
const std::vector<size_t>& NDArray<T>::shape() const {
    return dimensions;
}

// -----------------------------------------------------------------------------

// Get the total number of elements in the array
template<typename T>
size_t NDArray<T>::size() const {
    return data.size();
}

// -----------------------------------------------------------------------------

// Fill the array with a specific value
template<typename T>
void NDArray<T>::fill(const T& value) {
    std::fill(data.begin(), data.end(), value);
}

// -----------------------------------------------------------------------------

// Flatten the array
template<typename T>
NDArray<T> NDArray<T>::flatten() const {
    return NDArray<T>({data.size()}, std::vector<T>(data));
}

// -----------------------------------------------------------------------------

// Create an NDArray with given dimensions initialized with default-initialized elements
template<typename T>
NDArray<T> NDArrayFactory<T>::create(const std::vector<size_t>& dimensions) {
    size_t total = std::accumulate(
        dimensions.begin(), dimensions.end(), 1, std::multiplies<size_t>());
    return NDArray<T>(dimensions, std::vector<T>(total));
}

// -----------------------------------------------------------------------------

// Create an NDArray with given dimensions initialized with a specific value
template<typename T>
NDArray<T> NDArrayFactory<T>::create(const std::vector<size_t>& dimensions, const T& initValue) {
    size_t total = std::accumulate(
        dimensions.begin(), dimensions.end(), 1, std::multiplies<size_t>());
    return NDArray<T>(dimensions, std::vector<T>(total, initValue));
}

// -----------------------------------------------------------------------------

// Element-wise addition
template<typename T>
NDArray<T> operator+(const NDArray<T>& lhs, const NDArray<T>& rhs) {
    if (lhs.shape() != rhs.shape()) {
        throw std::invalid_argument("Shapes do not match");
    }
    NDArray<T> result(lhs.shape());
    std::transform(lhs.data.begin(), lhs.data.end(), rhs.data.begin(), result.data.begin(), std::plus<T>());
    return result;
}

// -----------------------------------------------------------------------------

// Element-wise subtraction
template<typename T>
NDArray<T> operator-(const NDArray<T>& lhs, const NDArray<T>& rhs) {
    if (lhs.shape() != rhs.shape()) {
        throw std::invalid_argument("Shapes do not match");
    }
    NDArray<T> result(lhs.shape());
    std::transform(lhs.data.begin(), lhs.data.end(), rhs.data.begin(), result.data.begin(), std::minus<T>());
    return result;
}

// -----------------------------------------------------------------------------

// Element-wise multiplication
template<typename T>
NDArray<T> operator*(const NDArray<T>& lhs, const NDArray<T>& rhs) {
    if (lhs.shape() != rhs.shape()) {
        throw std::invalid_argument("Shapes do not match");
    }
    NDArray<T> result(lhs.shape());
    std::transform(lhs.data.begin(), lhs.data.end(), rhs.data.begin(), result.data.begin(), std::multiplies<T>());
    return result;
}

// -----------------------------------------------------------------------------

// Element-wise division
template<typename T>
NDArray<T> operator/(const NDArray<T>& lhs, const NDArray<T>& rhs) {
    if (lhs.shape() != rhs.shape()) {
        throw std::invalid_argument("Shapes do not match");
    }
    NDArray<T> result(lhs.shape());
    std::transform(lhs.data.begin(), lhs.data.end(), rhs.data.begin(), result.data.begin(), std::divides<T>());
    return result;
}

// -----------------------------------------------------------------------------

}