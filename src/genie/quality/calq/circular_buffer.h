/**
 * Copyright 2018-2024 The Genie Authors.
 * @file circular_buffer.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of a template-based CircularBuffer class.
 *
 * This file contains the declaration of the `CircularBuffer` class, which
 * implements a fixed-Size circular buffer data structure. The buffer can be
 * used to store and access elements in a circular manner, overwriting the
 * oldest elements as new ones are added. This is useful for scenarios where a
 * fixed-Size queue-like structure is needed with fast access to both ends.
 *
 * @details The `CircularBuffer` class provides efficient access to elements
 * using standard subscript operators (`[]`), and supports methods to retrieve
 * and overwrite the oldest elements. It is implemented using a template to
 * allow storage of any data type, and uses a `std::vector` for the underlying
 * data storage.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_CIRCULAR_BUFFER_H_
#define SRC_GENIE_QUALITY_CALQ_CIRCULAR_BUFFER_H_

// -----------------------------------------------------------------------------

#include <cstddef>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

/**
 * @brief A fixed-Size buffer implemented in a circular manner.
 *
 * The `CircularBuffer` class provides a circular buffer of a fixed Size, where
 * newly added elements overwrite the oldest ones when the buffer is full. It
 * supports random access using the `[]` operator, and provides methods to
 * retrieve the oldest and newest elements in the buffer.
 *
 * @tparam T The type of elements to store in the circular buffer.
 */
template <typename T>
class CircularBuffer {
  std::vector<T>
      data_;    //!< @brief The underlying data storage for the circular buffer.
  size_t pos_;  //!< @brief The current position of the buffer's oldest element.

 public:
  /**
   * @brief Constructs a CircularBuffer with the specified Size and initial
   * value.
   *
   * This constructor initializes a circular buffer of the given Size and fills
   * it with the specified initial value for all elements.
   *
   * @param size The Size of the buffer.
   * @param val The initial value to fill the buffer with.
   */
  CircularBuffer(size_t size, const T& val) : pos_(0) {
    data_.resize(size, val);
  }

  /**
   * @brief Retrieves an element at the specified index.
   *
   * This subscript operator provides access to an element at the specified
   * index in the circular buffer. The index is relative to the current position
   * of the buffer, meaning `index = 0` refers to the oldest element in the
   * buffer.
   *
   * @param index The index of the element to retrieve.
   * @return A reference to the element at the specified index.
   */
  T& operator[](const size_t index) {
    return data_[(pos_ + index) % data_.size()];
  }

  /**
   * @brief Retrieves a constant element at the specified index.
   *
   * This const-qualified subscript operator provides read-only access to an
   * element at the specified index.
   *
   * @param index The index of the element to retrieve.
   * @return A constant reference to the element at the specified index.
   */
  const T& operator[](const size_t index) const {
    return data_[(pos_ + index) % data_.size()];
  }

  /**
   * @brief Retrieves the oldest element in the buffer.
   *
   * This method returns a reference to the oldest element currently stored in
   * the circular buffer. The oldest element is the one that will be overwritten
   * on the next `push()` operation.
   *
   * @return A reference to the oldest element in the buffer.
   */
  T& back() { return (*this)[pos_]; }

  /**
   * @brief Retrieves the oldest element in the buffer (const version).
   *
   * This method returns a constant reference to the oldest element currently
   * stored in the circular buffer.
   *
   * @return A constant reference to the oldest element in the buffer.
   */
  [[nodiscard]] const T& back() const { return (*this)[pos_]; }

  /**
   * @brief Retrieves the newest element in the buffer.
   *
   * This method returns a reference to the most recently added element in the
   * circular buffer.
   *
   * @return A reference to the newest element in the buffer.
   */
  [[nodiscard]] T& front() {
    return (*this)[(pos_ + data_.size() - 1) % data_.size()];
  }

  /**
   * @brief Retrieves the newest element in the buffer (const version).
   *
   * This method returns a constant reference to the most recently added element
   * in the circular buffer.
   *
   * @return A constant reference to the newest element in the buffer.
   */
  [[nodiscard]] const T& front() const {
    return (*this)[(pos_ + data_.size() - 1) % data_.size()];
  }

  /**
   * @brief Gets the Size of the circular buffer.
   *
   * This method returns the fixed Size of the circular buffer, which is
   * constant after the buffer is constructed.
   *
   * @return The Size of the buffer.
   */
  [[nodiscard]] size_t size() const { return data_.size(); }

  /**
   * @brief Adds a new element to the buffer, overwriting the oldest element.
   *
   * This method inserts a new element into the circular buffer, overwriting the
   * oldest element. After the insertion, the position of the oldest element is
   * updated to the next element in the buffer.
   *
   * @param val The new value to insert into the buffer.
   * @return The old value that was overwritten.
   */
  T push(const T& val) {
    T old_val = data_[pos_];
    data_[pos_] = val;
    pos_ = (pos_ + 1) % data_.size();
    return old_val;
  }
};

// -----------------------------------------------------------------------------
}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
#endif  // SRC_GENIE_QUALITY_CALQ_CIRCULAR_BUFFER_H_
