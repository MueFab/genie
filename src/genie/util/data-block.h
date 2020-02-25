/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_DATA_BLOCK_H_
#define GABAC_DATA_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include "exceptions.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

struct BlockStepper;

/**
 * @brief Word size aware data structure similar to a std::vector. Memory
 * efficient.
 */
class DataBlock {
   private:
    uint8_t lgWordSize;

    std::vector<uint8_t> data;

   public:
    /**
     * @brief Get lg base 2 of the size of one symbol in bytes
     * @return lg2 bytes of one symbol
     */
    uint8_t getLgWordSize() const { return lgWordSize; }

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
    int getWordSize() const { return 1 << lgWordSize; }

    /**
     * @brief multiply by size of one symbol in bytes
     * @return multiply arg by size of one symbol
     */
    uint64_t mulByWordSize(uint64_t val) const { return val << lgWordSize; }

    /**
     * @brief divide by size of one symbol in bytes
     * @return divide arg by size of one symbol
     */
    uint64_t divByWordSize(uint64_t val) const { return val >> lgWordSize; }

    /**
     * @brief modulo divide by size of one symbol in bytes
     * @return modulo divide arg by size of one symbol
     */
    uint64_t modByWordSize(uint64_t val) const { return val & ((1 << lgWordSize) - 1); }

    /**
     * @brief Creates a blockStepper for this DataBlock.
     * @warning It will become invalid once you add or remove elements from the
     * data block.
     * @todo This has to be refactored. There should be a BlockStepper_Const.
     * Currently it is possible to alter a const DataBlock via blockstepper
     * which should not be possible.
     * @return BlockStepper
     */
    BlockStepper getReader() const;

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
    uint64_t get(size_t index) const;

    /**
     * Calculates the biggest symbol in this block
     * @return Maximum
     */
    uint64_t getMaximum() const;

    /**
     * Calculates the biggest possible word size for this block
     * @return Maximum
     */
    uint8_t getMaxWordSize() const;

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
        T stream;
        size_t position;

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

    using Proxy = ProxyCore<DataBlock *>;            /**< Standard proxy */
    using ConstProxy = ProxyCore<const DataBlock *>; /**< Standard proxy for const */

    /**
     * @brief Iterator for data blocks. Like for proxy object: only use if
     * BlockStepper does not work.
     * @tparam T Data block type
     */
    template <typename T>
    class IteratorCore {
       private:
        T stream;
        size_t position;

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
        size_t operator-(const IteratorCore &offset) const { return position - offset.position; }

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
        size_t getOffset() const;

        /**
         * @brief Return DataBlock
         * @return Saved block
         */
        T getStream() const;

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

        using iterator_category = std::random_access_iterator_tag; /**< @brief Iterator category for
                                                                      STL */
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
    size_t size() const;

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
    bool empty() const;

    /**
     * @brief Begin const iterator
     * @return Const iterator to first element.
     */
    ConstIterator begin() const;

    /**
     * @brief Begin iterator
     * @return iterator to first element.
     */
    Iterator begin();

    /**
     * @brief End const iterator
     * @return Const iterator to behind the last element.
     */
    ConstIterator end() const;

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
    const void *getData() const;

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
    size_t getRawSize() const { return getWordSize() * size(); }

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
    explicit DataBlock(size_t size = 0, size_t wsize = 1);

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

inline uint64_t DataBlock::get(size_t index) const {
    switch (lgWordSize) {
        case 0:
            return *(data.data() + index);
        case 1:
            return *reinterpret_cast<const uint16_t *>(data.data() + (index << 1u));
        case 2:
            return *reinterpret_cast<const uint32_t *>(data.data() + (index << 2u));
        case 3:
            return *reinterpret_cast<const uint64_t *>(data.data() + (index << 3u));
        default:
            return 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

inline void DataBlock::set(size_t index, uint64_t val) {
    switch (lgWordSize) {
        case 0:
            *(data.data() + index) = static_cast<uint8_t>(val);
            return;
        case 1:
            *reinterpret_cast<uint16_t *>(data.data() + (index << 1u)) = static_cast<uint16_t>(val);
            return;
        case 2:
            *reinterpret_cast<uint32_t *>(data.data() + (index << 2u)) = static_cast<uint32_t>(val);
            return;
        case 3:
            *reinterpret_cast<uint64_t *>(data.data() + (index << 3u)) = static_cast<uint64_t>(val);
            return;
        default:
            return;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

inline DataBlock::ConstIterator DataBlock::begin() const { return {this, 0}; }

// ---------------------------------------------------------------------------------------------------------------------

inline DataBlock::Iterator DataBlock::begin() { return {this, 0}; }

// ---------------------------------------------------------------------------------------------------------------------

inline DataBlock::ConstIterator DataBlock::end() const { return {this, divByWordSize(data.size())}; }

// ---------------------------------------------------------------------------------------------------------------------

inline DataBlock::Iterator DataBlock::end() { return {this, divByWordSize(data.size())}; }

// ---------------------------------------------------------------------------------------------------------------------

inline void DataBlock::push_back(uint64_t val) {
    data.resize(data.size() + getWordSize());
    switch (lgWordSize) {
        case 0:
            *reinterpret_cast<uint8_t *>(data.end().base() - 1) = static_cast<uint8_t>(val);
            return;
        case 1:
            *reinterpret_cast<uint16_t *>(data.end().base() - 2) = static_cast<uint16_t>(val);
            return;
        case 2:
            *reinterpret_cast<uint32_t *>(data.end().base() - 4) = static_cast<uint32_t>(val);
            return;
        case 3:
            *reinterpret_cast<uint64_t *>(data.end().base() - 8) = static_cast<uint64_t>(val);
            return;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

inline void DataBlock::emplace_back(uint64_t val) { push_back(val); }

// ---------------------------------------------------------------------------------------------------------------------

inline const void *DataBlock::getData() const { return data.data(); }

// ---------------------------------------------------------------------------------------------------------------------

inline void *DataBlock::getData() { return data.data(); }

// ---------------------------------------------------------------------------------------------------------------------

inline void DataBlock::setWordSize(uint8_t size) {
    switch (size) {
        case 1:
            lgWordSize = 0;
            break;
        case 2:
            lgWordSize = 1;
            break;
        case 4:
            lgWordSize = 2;
            break;
        case 8:
            lgWordSize = 3;
            break;
        default:
            UTILS_DIE("Bad DataBlock word size");
    }
    if (modByWordSize(data.size())) {
        UTILS_DIE("Bad DataBlock word size");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline DataBlock::ProxyCore<T>::ProxyCore(T str, size_t pos) : stream(str), position(pos) {}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline DataBlock::ProxyCore<T>::operator uint64_t() const {
    return stream->get(position);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline DataBlock::ProxyCore<T> &DataBlock::ProxyCore<T>::operator=(uint64_t val) {
    stream->set(position, val);
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
DataBlock::IteratorCore<T>::IteratorCore(T str, size_t pos) : stream(str), position(pos) {}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline DataBlock::IteratorCore<T> DataBlock::IteratorCore<T>::operator+(size_t offset) const {
    return IteratorCore(stream, position + offset);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline DataBlock::IteratorCore<T> DataBlock::IteratorCore<T>::operator-(size_t offset) const {
    return IteratorCore(stream, position - offset);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline DataBlock::IteratorCore<T> &DataBlock::IteratorCore<T>::operator++() {
    *this = *this + 1;
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline DataBlock::IteratorCore<T> &DataBlock::IteratorCore<T>::operator--() {
    *this = *this + 1;
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline const DataBlock::IteratorCore<T> DataBlock::IteratorCore<T>::operator++(int) {
    IteratorCore ret = *this;
    ++(*this);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline const DataBlock::IteratorCore<T> DataBlock::IteratorCore<T>::operator--(int) {
    IteratorCore ret = *this;
    ++(*this);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline size_t DataBlock::IteratorCore<T>::getOffset() const {
    return position;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline T DataBlock::IteratorCore<T>::getStream() const {
    return stream;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline DataBlock::ProxyCore<T> DataBlock::IteratorCore<T>::operator*() const {
    return {stream, position};
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline bool DataBlock::IteratorCore<T>::operator==(const IteratorCore &c) const {
    return this->stream == c.stream && this->position == c.position;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline bool DataBlock::IteratorCore<T>::operator!=(const IteratorCore &c) const {
    return !(*this == c);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename IT1, typename IT2>
void DataBlock::insert(const IT1 &pos, const IT2 &start, const IT2 &end) {
    if (pos.getStream() != this || start.getStream() != end.getStream()) {
        return;
    }
    data.insert(data.begin() + pos.getOffset(), start.getStream()->data.begin() + start.getOffset(),
                end.getStream()->data.begin() + end.getOffset());
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
DataBlock::DataBlock(std::vector<T> *vec) {
    setWordSize(sizeof(T));
    size_t size = vec->size() * sizeof(T);
    this->data.resize(size);
    this->data.shrink_to_fit();
    std::memcpy(this->data.data(), vec->data(), size);
    vec->clear();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_DATA_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
