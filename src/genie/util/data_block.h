/**
 * Copyright 2018-2024 The Genie Authors.
 * @file data_block.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the DataBlock class for managing memory-efficient data
 * blocks.
 *
 * This file contains the declaration of the `DataBlock` class, which provides a
 * memory-efficient data structure for storing data in variable-sized symbols.
 * It offers functionality similar to `std::vector`, but with a focus on
 * handling data in non-standard element sizes using bit-packing and
 * word-Size-aware operations.
 *
 * @details The `DataBlock` class is optimized for applications that require
 * fine-grained control over memory usage and data alignment. It supports custom
 * word sizes, symbol extraction, insertion, and iterator-based access. The
 * class is designed to be used in scenarios where standard containers like
 * `std::vector` are not sufficient due to non-standard element sizes.
 */

#ifndef SRC_GENIE_UTIL_DATA_BLOCK_H_
#define SRC_GENIE_UTIL_DATA_BLOCK_H_

// -----------------------------------------------------------------------------

#include <string>
#include <utility>
#include <vector>

#include "genie/util/block_stepper.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

/**
 * @brief Memory-efficient data structure similar to `std::vector` with support
 * for custom word sizes.
 *
 * The `DataBlock` class provides a container for storing data using a custom
 * word Size. It allows for efficient memory management and provides operations
 * for manipulating individual elements using word-Size-aware methods. It
 * includes support for bit-packing, symbol extraction, insertion, and iterators
 * for accessing the elements.
 */
class DataBlock {
  /// log2 of the word size. The word Size is defined as 1 << lgWordSize.
  uint8_t lg_word_size_;

  /// The raw data stored as a vector of bytes.
  std::vector<uint8_t> data_;

 public:
  /**
   * @brief Sets the Size of one symbol, changing the number of elements.
   *
   * This function changes the word Size used for each symbol in the
   * `DataBlock`. It modifies the internal state to adjust the number of
   * elements based on the new word Size.
   *
   * @param size The new Size of each symbol in bytes.
   */
  void SetWordSize(uint8_t size);

  /**
   * @brief Get the Size of one symbol in bytes.
   *
   * This function returns the Size of each symbol in bytes. The return type
   * is `uint8_t`, but internally it uses an integer representation to avoid
   * issues with C/C++ type promotion.
   *
   * @return The Size of one symbol in bytes.
   */
  [[nodiscard]] uint8_t GetWordSize() const;

  /**
   * @brief Multiplies a value by the Size of one symbol in bytes.
   *
   * This function multiplies the given value by the word Size in bytes.
   *
   * @param val The value to be multiplied.
   * @return The result of the multiplication.
   */
  [[nodiscard]] uint64_t MulByWordSize(uint64_t val) const;

  /**
   * @brief Divides a value by the Size of one symbol in bytes.
   *
   * This function divides the given value by the word Size in bytes.
   *
   * @param val The value to be divided.
   * @return The result of the division.
   */
  [[nodiscard]] uint64_t DivByWordSize(uint64_t val) const;

  /**
   * @brief Calculates the remainder when dividing by the Size of one symbol
   * in bytes.
   *
   * This function returns the remainder when the given value is divided by
   * the word Size in bytes.
   *
   * @param val The value to be used in the modulo operation.
   * @return The remainder of the division.
   */
  [[nodiscard]] uint64_t ModByWordSize(uint64_t val) const;

  /**
   * @brief Creates a `BlockStepper` for this `DataBlock`.
   *
   * This function creates a `BlockStepper` object that can be used to iterate
   * over the elements in the `DataBlock`. The `BlockStepper` provides an
   * efficient way to access and manipulate data within the `DataBlock`.
   *
   * @warning The `BlockStepper` will become invalid if elements are added or
   * removed from the `DataBlock`.
   * @return A `BlockStepper` object for iterating over the `DataBlock`.
   */
  [[nodiscard]] BlockStepper GetReader() const;

  /**
   * @brief Compares the contents and word Size of two `DataBlock` objects.
   *
   * This function checks if the data and word Size of the current `DataBlock`
   * are equal to those of another `DataBlock` object.
   *
   * @param d The other `DataBlock` to compare.
   * @return True if the `DataBlock` objects are equal, false otherwise.
   */
  bool operator==(const DataBlock& d) const;

  /**
   * @brief Sets the contents of the `DataBlock` using an initializer list.
   *
   * This function replaces the contents of the `DataBlock` with the values
   * from the given initializer list. It does not modify the word Size.
   *
   * @param il The initializer list containing the new values.
   * @return A reference to the updated `DataBlock`.
   */
  DataBlock& operator=(const std::initializer_list<uint64_t>& il);

  /**
   * @brief Extracts one symbol from the `DataBlock`.
   *
   * This function retrieves the symbol at the specified index and returns it
   * as a 64-bit value.
   *
   * @param index The position of the symbol in the `DataBlock`.
   * @return The symbol at the specified position, widened to 64 bits.
   */
  [[nodiscard]] uint64_t Get(size_t index) const;

  /**
   * @brief Sets the value of a symbol at the specified index.
   *
   * This function updates the symbol at the given index with a new value.
   * The value will be narrowed to fit the word Size.
   *
   * @param index The position of the symbol to update.
   * @param val The new value to be set.
   */
  void Set(size_t index, uint64_t val);

  // Inner classes Begin here

  /**
   * @brief Proxy class for abstracting access to individual elements using
   * operators.
   *
   * The `ProxyCore` class provides a way to access and manipulate individual
   * elements in the `DataBlock` using standard operators such as assignment
   * and dereferencing. It allows for operations like range-based for loops
   * and use with STL algorithms.
   *
   * @tparam T The type of the `DataBlock` being accessed.
   */
  template <typename T>
  class ProxyCore {
    /// The `DataBlock` being referred to.
    T stream_;

    /// The position inside the `DataBlock`.
    size_t position_;

   public:
    /**
     * @brief Creates a `ProxyCore` object for accessing a specific element
     * in the `DataBlock`.
     *
     * @param str The `DataBlock` being referred to.
     * @param pos The index of the element inside the `DataBlock`.
     */
    ProxyCore(T str, size_t pos);

    /**
     * @brief Conversion to `uint64_t` by invoking `get()` on the
     * `DataBlock`.
     *
     * This function allows the `ProxyCore` to be implicitly converted to a
     * `uint64_t` by invoking the `get()` method of the associated
     * `DataBlock`.
     *
     * @return The value of the element at the current position.
     */
    explicit operator uint64_t() const;

    /**
     * @brief Assigns a new value to the current element using `set()` of
     * the `DataBlock`.
     *
     * This function updates the value of the element at the current
     * position using the `set()` method of the associated `DataBlock`.
     *
     * @param val The new value to be assigned to the element.
     * @return A reference to the updated `ProxyCore` object.
     */
    ProxyCore& operator=(uint64_t val);
  };

  /**
   * @brief Iterator class for traversing elements in the `DataBlock`.
   *
   * The `IteratorCore` class provides an iterator interface for accessing the
   * elements of the `DataBlock`. It supports standard iterator operations
   * like Increment, decrement, and random access, making it compatible with
   * STL algorithms.
   *
   * @tparam T The type of `DataBlock` being iterated over.
   */
  template <typename T>
  class IteratorCore {
    /// The `DataBlock` being iterated over.
    T stream_;

    /// The position of the current element inside the `DataBlock`.
    size_t position_;

   public:
    /**
     * @brief Constructs an iterator for the `DataBlock` starting at the
     * specified position.
     *
     * @param str The `DataBlock` to iterate over.
     * @param pos The initial position of the iterator.
     */
    IteratorCore(T str, size_t pos);

    /**
     * @brief Advances the iterator by a specified offset.
     *
     * @param offset The number of elements to advance.
     * @return A new `IteratorCore` pointing to the updated position.
     */
    IteratorCore operator+(size_t offset) const;

    /**
     * @brief Rewinds the iterator by a specified offset.
     *
     * @param offset The number of elements to rewind.
     * @return A new `IteratorCore` pointing to the updated position.
     */
    IteratorCore operator-(size_t offset) const;

    /**
     * @brief Calculates the offset between two iterators.
     *
     * This function returns the difference between the current iterator and
     * another iterator, indicating the number of elements between them.
     *
     * @param offset The other iterator to compare.
     * @return The number of elements between the iterators.
     */
    size_t operator-(const IteratorCore& offset) const;

    /**
     * @brief Prefix Increment operator.
     *
     * Advances the iterator to the next element.
     *
     * @return A reference to the updated iterator.
     */
    IteratorCore& operator++();

    /**
     * @brief Prefix decrement operator.
     *
     * Moves the iterator to the previous element.
     *
     * @return A reference to the updated iterator.
     */
    IteratorCore& operator--();

    /**
     * @brief Postfix Increment operator.
     *
     * Advances the iterator to the next element.
     *
     * @return The iterator before the Increment.
     */
    IteratorCore operator++(int);

    /**
     * @brief Postfix decrement operator.
     *
     * Moves the iterator to the previous element.
     *
     * @return The iterator before the decrement.
     */
    IteratorCore operator--(int);

    /**
     * @brief Returns the current index position of the iterator.
     *
     * @return The index position inside the `DataBlock`.
     */
    [[maybe_unused]] [[nodiscard]] size_t GetOffset() const;

    /**
     * @brief Dereferences the iterator to a `ProxyCore` object.
     *
     * This function returns a `ProxyCore` object that abstracts access to
     * the current element in the `DataBlock`.
     *
     * @return A `ProxyCore` object for the current element.
     */
    ProxyCore<T> operator*() const;

    /**
     * @brief Equality comparison operator.
     *
     * Compares two iterators for equality based on the `DataBlock` and
     * position.
     *
     * @param c The other iterator to compare.
     * @return True if both iterators refer to the same element, false
     * otherwise.
     */
    bool operator==(const IteratorCore& c) const;

    /**
     * @brief Inequality comparison operator.
     *
     * Compares two iterators for inequality based on the `DataBlock` and
     * position.
     *
     * @param c The other iterator to compare.
     * @return True if the iterators do not refer to the same element, false
     * otherwise.
     */
    bool operator!=(const IteratorCore& c) const;

    using IteratorCategory [[maybe_unused]] = std::random_access_iterator_tag;
    using Reference = ProxyCore<T>;
    using Pointer = ProxyCore<T>*;
    using ValueType = ProxyCore<T>;
    using DifferenceType [[maybe_unused]] = size_t;
  };

  using Iterator = IteratorCore<DataBlock*>;
  using ConstIterator = IteratorCore<const DataBlock*>;

  /**
   * @brief Get number of elements in the `DataBlock`.
   *
   * This function returns the number of elements currently stored in the
   * `DataBlock`.
   *
   * @return The Size of the `DataBlock` in terms of elements.
   */
  [[nodiscard]] size_t Size() const;

  /**
   * @brief Reserves more memory without expanding the actual data.
   *
   * This function increases the capacity of the `DataBlock` to accommodate
   * the specified number of elements without changing the current Size of the
   * `DataBlock`.
   *
   * @param size The new memory capacity in terms of elements.
   */
  [[maybe_unused]] void Reserve(size_t size);

  /**
   * @brief Deletes all elements in the `DataBlock`.
   *
   * This function clears the contents of the `DataBlock` and sets its Size to
   * zero.
   */
  void Clear();

  /**
   * @brief Resizes the `DataBlock` to a specified number of elements.
   *
   * If the `DataBlock` is resized to a smaller Size, the excess elements are
   * discarded. If resized to a larger Size, new elements are initialized with
   * default values.
   *
   * @param size The new Size in terms of elements.
   */
  void Resize(size_t size);

  /**
   * @brief Checks if the `DataBlock` is empty.
   *
   * This function checks if there are any elements in the `DataBlock`.
   *
   * @return True if there are no elements, false otherwise.
   */
  [[nodiscard]] bool Empty() const;

  /**
   * @brief Begin const iterator.
   *
   * Returns a const iterator to the first element in the `DataBlock`.
   *
   * @return A const iterator to the first element.
   */
  [[nodiscard]] ConstIterator begin() const;

  /**
   * @brief Begin mutable iterator.
   *
   * Returns an iterator to the first element in the `DataBlock`.
   *
   * @return An iterator to the first element.
   */
  Iterator begin();

  /**
   * @brief End const iterator.
   *
   * Returns a const iterator to the position past the last element in the
   * `DataBlock`.
   *
   * @return A const iterator to the position past the last element.
   */
  [[nodiscard]] ConstIterator end() const;

  /**
   * @brief End mutable iterator.
   *
   * Returns an iterator to the position past the last element in the
   * `DataBlock`.
   *
   * @return An iterator to the position past the last element.
   */
  Iterator end();

  /**
   * @brief Appends a new symbol to the `DataBlock`.
   *
   * This function adds a new symbol to the end of the `DataBlock`.
   *
   * @param val The value of the new symbol to be added.
   */
  void PushBack(uint64_t val);

  /**
   * @brief Emplaces a new symbol into the `DataBlock`.
   *
   * This function constructs a new symbol at the end of the `DataBlock`.
   *
   * @param val The value of the new symbol to be constructed.
   */
  [[maybe_unused]] void EmplaceBack(uint64_t val);

  /**
   * @brief Gets a raw const pointer to the memory block.
   *
   * This function returns a raw pointer to the underlying memory block,
   * allowing for low-level access to the data.
   *
   * @return A const pointer to the memory block.
   */
  [[nodiscard]] const void* GetData() const;

  /**
   * @brief Gets a raw pointer to the memory block.
   *
   * This function returns a raw pointer to the underlying memory block,
   * allowing for low-level access to the data.
   *
   * @return A pointer to the memory block.
   */
  void* GetData();

  /**
   * @brief Gets the Size of the underlying data in bytes.
   *
   * This function returns the total Size of the data stored in the
   * `DataBlock` in bytes, which is computed as `wordsize * numberOfElements`.
   *
   * @return The total Size of the data in bytes.
   */
  [[nodiscard]] size_t GetRawSize() const;

  /**
   * @brief Swaps the contents of two `DataBlock` objects.
   *
   * This function exchanges the contents of the current `DataBlock` with
   * those of another `DataBlock` without performing a copy operation, making
   * it efficient.
   *
   * @param d The other `DataBlock` to swap with.
   */
  void Swap(DataBlock* d);

  /**
   * @brief Inserts elements into the `DataBlock`.
   *
   * This function inserts a range of elements into the `DataBlock` at the
   * specified position.
   *
   * @tparam It1 The type of the iterator for the insertion position.
   * @tparam It2 The type of the iterator for the range of elements.
   * @param pos The position at which to insert the elements.
   * @param start The start of the range of elements to be inserted.
   * @param end The end of the range of elements to be inserted.
   */
  template <typename It1, typename It2>
  [[maybe_unused]] void Insert(const It1& pos, const It2& start,
                               const It2& end);

  /**
   * @brief Creates a `DataBlock` with a specified initial Size and word size.
   *
   * This constructor initializes a `DataBlock` with the given Size and word
   * Size.
   *
   * @param size The initial Size in terms of elements.
   * @param word_size The Size of each element in bytes.
   */
  explicit DataBlock(size_t size = 0, uint8_t word_size = 1);

  /**
   * @brief Creates a `DataBlock` from a vector of values.
   *
   * This constructor initializes a `DataBlock` using the contents of a given
   * vector. It requires that the vector elements are of an integral type.
   *
   * @tparam T The type of the vector elements.
   * @param vec A pointer to the vector containing the values.
   */
  template <typename T>
  explicit DataBlock(std::vector<T>* vec);

  /**
   * @brief Creates a `DataBlock` from a vector of bytes.
   *
   * This is an optimized constructor that allows a vector of bytes to be
   * moved into the `DataBlock` without any copy operations.
   *
   * @param vec A pointer to the vector of bytes to be moved.
   */
  explicit DataBlock(std::vector<uint8_t>* vec);

  /**
   * @brief Creates a `DataBlock` from a string.
   *
   * This constructor treats the contents of the string as a vector of bytes
   * and initializes the `DataBlock` using the given string.
   *
   * @param vec A pointer to the string to be used as the source.
   */
  explicit DataBlock(std::string* vec);

  /**
   * @brief Creates a `DataBlock` from a raw memory buffer.
   *
   * This constructor initializes a `DataBlock` by copying the contents of a
   * given memory buffer.
   *
   * @param d A pointer to the start of the memory buffer.
   * @param size The number of elements in the buffer.
   * @param word_size The Size of each element.
   */
  explicit DataBlock(const uint8_t* d, size_t size, uint8_t word_size);
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#include "genie/util/data_block.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_DATA_BLOCK_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
