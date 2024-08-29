/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_DATA_BLOCK_H_
#define SRC_GENIE_UTIL_DATA_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <string>
#include <utility>
#include <vector>
#include "genie/util/block-stepper.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

struct BlockStepper;

/**
 * @brief Word size aware data structure similar to a std::vector. Memory
 * efficient.
 */
class DataBlock {
 private:
    uint8_t lgWordSize;         //!< @brief log2 of the wordsize. Wordsize = 1 << lgWordsize
    std::vector<uint8_t> data;  //!< @brief The actual raw data.

 public:
    /**
     * @brief Get lg base 2 of the size of one symbol in bytes
     * @return lg2 bytes of one symbol
     */
    [[nodiscard]] uint8_t getLgWordSize() const;

    /**
     * @brief Sets the size of one symbol, changing the number of elments
     * @param size New size in bytes
     */
    void setWordSize(uint8_t size);

    /**
     * @brief Get size of one symbol in bytes.
     * @return bytes of one symbol
     *
     * Note: This is declared to be int and not uint8_t on purpose!!
     * According to C/C++ semantics, operands smaller than one int are
     * promoted to int before being used in and expression.  If the return
     * type were uint8_t, then the result of the shift expression would be
     * narrowed to 8 bits (a useless and with 0xff what we know does nothing)
     * before promoting it back to int.  Ergo, just leave the return type
     * of the word size as int.
     */
    [[nodiscard]] uint8_t getWordSize() const;

    /**
     * @brief multiply by size of one symbol in bytes
     * @return multiply arg by size of one symbol
     */
    [[nodiscard]] uint64_t mulByWordSize(uint64_t val) const;

    /**
     * @brief divide by size of one symbol in bytes
     * @return divide arg by size of one symbol
     */
    [[nodiscard]] uint64_t divByWordSize(uint64_t val) const;

    /**
     * @brief modulo divide by size of one symbol in bytes
     * @return modulo divide arg by size of one symbol
     */
    [[nodiscard]] uint64_t modByWordSize(uint64_t val) const;

    /**
     * @brief Creates a blockStepper for this DataBlock.
     * @warning It will become invalid once you add or remove elements from the
     * data block.
     * @todo This has to be refactored. There should be a BlockStepper_Const.
     * Currently it is possible to alter a const DataBlock via blockstepper
     * which should not be possible.
     * @return BlockStepper
     */
    [[nodiscard]] BlockStepper getReader() const;

    /**
     * @brief Compare data and word size
     * @param d Other block
     * @return True if equal
     */
    bool operator==(const DataBlock &d) const;

    /**
     * @brief Set contents to values from a list. Does not affect the word size.
     * @param il List
     * @return This data block.
     */
    DataBlock &operator=(const std::initializer_list<uint64_t> &il);

    /**
     * @brief Extract one symbol
     * @param index Position of symbol in block
     * @return Symbol widened to 64 bits
     */
    [[nodiscard]] uint64_t get(size_t index) const;

    /**
     * @brief Calculates the biggest possible word size for this block
     * @return Maximum
     */
    [[nodiscard]] uint8_t getMaxWordSize() const;

    /**
     * @brief Pack symbol into stream.
     * @param index Position in stream
     * @param val Value (will be narrowed to word size)
     */
    void set(size_t index, uint64_t val);

    /**
     * @brief A proxy object abstracting the get/set access using regular
     * operators. This way it is possible to use for example range based for
     * loops or std algorithms with iterators.
     * @note Even though this is convenient you should avoid it. It's slow.
     * @warning Adding or removing elements from the data block this object is
     * referring to leads to undefined behaviour. Don't do it as long as a proxy
     * object is existent.
     * @tparam T Type of data block
     */
    template <typename T>
    class ProxyCore {
     private:
        T stream;         //!< @brief The DataBlock we are referring to.
        size_t position;  //!< @brief The position inside the datablock.

     public:
        /**
         * @brief Create access proxy
         * @param str Block we are referring to
         * @param pos Element index inside the data block
         */
        ProxyCore(T str, size_t pos);

        /**
         * @brief Conversion to uint64_t - executes get() of data block
         * @return str.get(pos);
         */
        explicit operator uint64_t() const;

        /**
         * @brief Assign uint64_t - executes set() of data block
         * @param val New value
         * @return *this, so that you can execute additional operations on this
         * element.
         */
        ProxyCore &operator=(uint64_t val);
    };

    /**
     * @brief Iterator for data blocks. Like for proxy object: only use if
     * BlockStepper does not work.
     * @tparam T Data block type
     */
    template <typename T>
    class IteratorCore {
     private:
        T stream;         //!< @brief The DataBlock we are referring to.
        size_t position;  //!< @brief The position inside the datablock.

     public:
        /**
         * @brief Create iterator
         * @param str Data block
         * @param pos Index in data block.
         */
        IteratorCore(T str, size_t pos);

        /**
         * @brief Fast forward
         * @param offset Index offset
         * @return *this
         */
        IteratorCore operator+(size_t offset) const;

        /**
         * @brief Rewind
         * @param offset Index offset
         * @return *this
         */
        IteratorCore operator-(size_t offset) const;

        /**
         * @brief Calculate offset between iterators
         * @param offset Other iterator
         * @return Index offset
         */
        size_t operator-(const IteratorCore &offset) const;

        /**
         * @brief Increment prefix
         * @return *this
         */
        IteratorCore &operator++();

        /**
         * @brief Decrement prefix
         * @return *this
         */
        IteratorCore &operator--();

        /**
         * @brief Increment postfix
         * @return *this
         */
        const IteratorCore operator++(int);

        /**
         * @brief Increment postfix
         * @return *this
         */
        const IteratorCore operator--(int);

        /**
         * @brief Return index
         * @return Saved index
         */
        [[nodiscard]] size_t getOffset() const;

        /**
         * @brief Dereference to Proxy object
         * @return Newly created proxy
         */
        ProxyCore<T> operator*() const;

        /**
         * @brief Compare
         * @param c other Iterator
         * @return True if block and index equal
         */
        bool operator==(const IteratorCore &c) const;

        /**
         * @brief Compare
         * @param c other Iterator
         * @return False if block and index equal
         */
        bool operator!=(const IteratorCore &c) const;

        using iterator_category = std::random_access_iterator_tag; /**< @brief Iterator category for  STL */
        using reference = ProxyCore<T>;                            /**< @brief Reference type for STL */
        using pointer = ProxyCore<T> *;                            /**< @brief Pointer type for STL */
        using value_type = ProxyCore<T>;                           /**< @brief Value type for STL */
        using difference_type = size_t;                            /**< @brief Difference type for STL */
    };

    using Iterator = IteratorCore<DataBlock *>;            /**< @brief Default iterator */
    using ConstIterator = IteratorCore<const DataBlock *>; /**< @brief Default const iterator */

    /**
     * @brief Get number of elements
     * @return Size of data blocks in elements
     */
    [[nodiscard]] size_t size() const;

    /**
     * @brief Reserve more memory without expanding the actual data. Similar to
     * vector.
     * @param size New memory size in elements
     */
    void reserve(size_t size);

    /**
     * @brief Free unused memory without shrinking the actual data. Similar to
     * vector.
     */
    void shrink_to_fit();

    /**
     * @brief Delete all elements. Sets the size to zero.
     */
    void clear();

    /**
     * @brief Sets the size to a specified number of elements.
     * If you are shrinking, the last elements are discarded.
     * @param size New size in elements
     */
    void resize(size_t size);

    /**
     * @brief Check if there are elements in the block.
     * @return True if no elements
     */
    [[nodiscard]] bool empty() const;

    /**
     * @brief Begin const iterator
     * @return Const iterator to first element.
     */
    [[nodiscard]] ConstIterator begin() const;

    /**
     * @brief Begin iterator
     * @return iterator to first element.
     */
    Iterator begin();

    /**
     * @brief End const iterator
     * @return Const iterator to behind the last element.
     */
    [[nodiscard]] ConstIterator end() const;

    /**
     * @brief End iterator
     * @return iterator to behind the last element.
     */
    Iterator end();

    /**
     * @brief Append a new symbol
     * @param val Value of symbol
     */
    void push_back(uint64_t val);

    /**
     * @brief Append a new symbol
     * @param val Value of symbol
     */
    void emplace_back(uint64_t val);

    /**
     * @brief Get raw const pointer to memory block
     * @return Pointer
     */
    [[nodiscard]] const void *getData() const;

    /**
     * @brief Get raw pointer to memory block
     * @return Pointer
     */
    void *getData();

    /**
     * @brief Gets the size of the underlying data in bytes.
     * wordsize * numberOfElements
     * @return Data size in bytes
     */
    [[nodiscard]] size_t getRawSize() const;

    /**
     * @brief Swap the contents of two data blocks without copying of data.
     * @param d The other block.
     */
    void swap(DataBlock *d);

    /**
     * @brief Insert elements into the data block
     * @tparam IT1 Type of iterator 1
     * @tparam IT2 Type of Iterator 2
     * @param pos Where to insert
     * @param start Where the source buffer starts
     * @param end Where the source buffer ends
     */
    template <typename IT1, typename IT2>
    void insert(const IT1 &pos, const IT2 &start, const IT2 &end);

    /**
     * @brief Create Data block
     * @param size initial size in elements
     * @param wsize size of one element in bytes
     */
    explicit DataBlock(size_t size = 0, uint8_t wsize = 1);

    /**
     * @brief Create Data block from vector
     * @tparam T Should be an integral type
     * @param vec Vector to process
     */
    template <typename T>
    explicit DataBlock(std::vector<T> *vec);

    /**
     * @brief Create Data block from vector.
     * This is an optimization, as you can move byte vectors
     * into data blocks without any copy.
     * @param vec Other vector
     */
    explicit DataBlock(std::vector<uint8_t> *vec);

    /**
     * @brief Create data block from string.
     * @param vec String treated like a vector here.
     */
    explicit DataBlock(std::string *vec);

    /**
     * @brief Create data block from a memory buffer. Will copy the data.
     * @param d Start of buffer
     * @param size Number of elements
     * @param word_size Size of one element
     */
    explicit DataBlock(const uint8_t *d, size_t size, uint8_t word_size);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/data-block.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_DATA_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
