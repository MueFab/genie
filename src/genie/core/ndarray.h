#ifndef GENIE_NDARRAY_H
#define GENIE_NDARRAY_H

// -----------------------------------------------------------------------------

#include <vector>
#include <stdexcept>
#include <cstddef>

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
    void computeStrides();          ///< Compute strides based on dimensions.

 public:
    /**
     * @brief Default constructor.
     *
     * Initializes an empty NDArray with no dimensions and no data.
     */
    NDArray();

    /**
     * @brief Constructor from dimensions and buffer.
     *
     * Initializes an NDArray with the given dimensions and data buffer.
     *
     * @param dims The dimensions of the array.
     * @param buffer The data buffer to initialize the array with.
     */
    NDArray(const std::vector<size_t>& dims, std::vector<T>&& buffer);

    /**
     * @brief Constructor from dimensions and a buffer by reference.
     *
     * Initializes an NDArray with the given dimensions and data buffer.
     *
     * @param dims The dimensions of the array.
     * @param buffer The data buffer to initialize the array with.
     */
    NDArray(const std::vector<size_t>& dims, const std::vector<T>& buffer);

    /**
     * @brief Move constructor.
     *
     * Move constructs an NDArray from another NDArray.
     *
     * @param other The NDArray to move from.
     */
    NDArray(NDArray&& other) noexcept;

    /**
     * @brief Copy constructor.
     *
     * Copy constructs an NDArray from another NDArray.
     *
     * @param other The NDArray to copy from.
     */
    NDArray(const NDArray& other);

    /**
     * @brief Move assignment operator.
     *
     * Move assigns an NDArray from another NDArray.
     *
     * @param other The NDArray to move from.
     * @return Reference to the current NDArray.
     */
    NDArray& operator=(NDArray&& other) noexcept;

    /**
     * @brief Copy assignment operator.
     *
     * Copy assigns an NDArray from another NDArray.
     *
     * @param other The NDArray to copy from.
     * @return Reference to the current NDArray.
     */
    NDArray& operator=(const NDArray& other);

    /**
     * @brief Resize the array.
     *
     * Resizes the array to the new dimensions. If the new size is larger, fills the new elements with the default value.
     * If the new size is smaller, truncates the array.
     *
     * @param newDims The new dimensions of the array.
     * @param defaultValue The default value to initialize new elements with.
     */
    void resize(const std::vector<size_t>& newDims, const T& defaultValue = T());

    /**
     * @brief Access element using vector of indices.
     *
     * Accesses the element at the specified indices.
     *
     * @param indices The indices of the element to access.
     * @return Reference to the element at the specified indices.
     * @throws std::invalid_argument If the number of indices does not match the number of dimensions.
     * @throws std::out_of_range If any index is out of range.
     */
    T& operator()(const std::vector<size_t>& indices);

    /**
     * @brief Variadic template version for convenient access.
     *
     * Accesses the element at the specified indices.
     *
     * @tparam Args Variadic template arguments representing the indices.
     * @param args The indices of the element to access.
     * @return Reference to the element at the specified indices.
     * @throws std::invalid_argument If the number of indices does not match the number of dimensions.
     * @throws std::out_of_range If any index is out of range.
     */
    template<typename... Args>
    T& operator()(Args... args);

    /**
     * @brief Const version of access element using vector of indices.
     *
     * Accesses the element at the specified indices (const version).
     *
     * @param indices The indices of the element to access.
     * @return Const reference to the element at the specified indices.
     * @throws std::invalid_argument If the number of indices does not match the number of dimensions.
     * @throws std::out_of_range If any index is out of range.
     */
    const T& operator()(const std::vector<size_t>& indices) const;

    /**
     * @brief Const variadic template version for convenient access.
     *
     * Accesses the element at the specified indices (const version).
     *
     * @tparam Args Variadic template arguments representing the indices.
     * @param args The indices of the element to access.
     * @return Const reference to the element at the specified indices.
     * @throws std::invalid_argument If the number of indices does not match the number of dimensions.
     * @throws std::out_of_range If any index is out of range.
     */
    template<typename... Args>
    const T& operator()(Args... args) const;

    /**
     * @brief Get the shape of the array.
     *
     * Returns the dimensions of the array.
     *
     * @return Const reference to the dimensions of the array.
     */
    const std::vector<size_t>& shape() const;

    /**
     * @brief Get the total number of elements in the array.
     *
     * Returns the total number of elements in the array.
     *
     * @return The total number of elements.
     */
    size_t size() const;

    /**
     * @brief Fill the array with a specific value.
     *
     * Fills all elements of the array with the specified value.
     *
     * @param value The value to fill the array with.
     */
    void fill(const T& value);

    /**
     * @brief Flatten the array.
     *
     * Flattens the multi-dimensional array into a 1D array.
     *
     * @return A new NDArray with a single dimension containing all elements.
     */
    NDArray<T> flatten() const;
};

// -----------------------------------------------------------------------------

/**
 * @brief Factory class for creating NDArray instances.
 *
 * This class provides static methods to create NDArray instances with specific dimensions
 * and initial values.
 */
template<typename T>
class NDArrayFactory {
 public:
    /**
     * @brief Create an NDArray with given dimensions initialized with default-initialized elements.
     *
     * Creates an NDArray with the specified dimensions, initializing all elements to their default values.
     *
     * @param dimensions The dimensions of the array.
     * @return An NDArray with the specified dimensions and default-initialized elements.
     */
    static NDArray<T> create(const std::vector<size_t>& dimensions);

    /**
     * @brief Create an NDArray with given dimensions initialized with a specific value.
     *
     * Creates an NDArray with the specified dimensions, initializing all elements to the given value.
     *
     * @param dimensions The dimensions of the array.
     * @param initValue The initial value for all elements.
     * @return An NDArray with the specified dimensions and initialized elements.
     */
    static NDArray<T> create(const std::vector<size_t>& dimensions, const T& initValue);
};

// -----------------------------------------------------------------------------

} // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // GENIE_NDARRAY_H

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
