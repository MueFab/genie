/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_DATA_BLOCK_IMPL_H_
#define SRC_GENIE_UTIL_DATA_BLOCK_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

inline uint8_t DataBlock::getLgWordSize() const { return lgWordSize; }

// ---------------------------------------------------------------------------------------------------------------------

inline uint8_t DataBlock::GetWordSize() const { return static_cast<uint8_t>(1 << lgWordSize); }

// ---------------------------------------------------------------------------------------------------------------------

inline uint64_t DataBlock::MulByWordSize(uint64_t val) const { return val << lgWordSize; }

// ---------------------------------------------------------------------------------------------------------------------

inline uint64_t DataBlock::DivByWordSize(uint64_t val) const { return val >> lgWordSize; }

// ---------------------------------------------------------------------------------------------------------------------

inline uint64_t DataBlock::ModByWordSize(uint64_t val) const {
    return val & ((1 << static_cast<uint64_t> (lgWordSize)) - 1);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline size_t DataBlock::IteratorCore<T>::operator-(const IteratorCore &offset) const {
    return position - offset.position;
}

// ---------------------------------------------------------------------------------------------------------------------

inline size_t DataBlock::GetRawSize() const { return GetWordSize() * size(); }

// ---------------------------------------------------------------------------------------------------------------------

inline uint64_t DataBlock::Get(size_t index) const {
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

inline void DataBlock::Set(size_t index, uint64_t val) {
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

inline DataBlock::ConstIterator DataBlock::end() const { return {this, size_t(DivByWordSize(data.size()))}; }

// ---------------------------------------------------------------------------------------------------------------------

inline DataBlock::Iterator DataBlock::end() { return {this, size_t(DivByWordSize(data.size()))}; }

// ---------------------------------------------------------------------------------------------------------------------

inline void DataBlock::PushBack(uint64_t val) {
    data.resize(data.size() + GetWordSize());
    switch (lgWordSize) {
        case 0:
            *(data.end() - 1) = static_cast<uint8_t>(val);
            return;
        case 1:
            *reinterpret_cast<uint16_t *>(&*(data.end() - 2)) = static_cast<uint16_t>(val);
            return;
        case 2:
            *reinterpret_cast<uint32_t *>(&*(data.end() - 4)) = static_cast<uint32_t>(val);
            return;
        case 3:
            *reinterpret_cast<uint64_t *>(&*(data.end() - 8)) = static_cast<uint64_t>(val);
            return;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

inline void DataBlock::EmplaceBack(uint64_t val) { PushBack(val); }

// ---------------------------------------------------------------------------------------------------------------------

inline const void *DataBlock::GetData() const { return data.data(); }

// ---------------------------------------------------------------------------------------------------------------------

inline void *DataBlock::GetData() { return data.data(); }

// ---------------------------------------------------------------------------------------------------------------------

inline void DataBlock::SetWordSize(uint8_t size) {
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
    if (ModByWordSize(data.size())) {
        UTILS_DIE("Bad DataBlock word size");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline DataBlock::ProxyCore<T>::ProxyCore(T str, size_t pos) : stream(str), position(pos) {}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline DataBlock::ProxyCore<T>::operator uint64_t() const {
    return stream->Get(position);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
inline DataBlock::ProxyCore<T> &DataBlock::ProxyCore<T>::operator=(uint64_t val) {
    stream->Set(position, val);
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
inline size_t DataBlock::IteratorCore<T>::GetOffset() const {
    return position;
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
    return !(*this == c);  // NOLINT
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename IT1, typename IT2>
void DataBlock::Insert(const IT1 &pos, const IT2 &start, const IT2 &end) {
    if (pos.getStream() != this || start.getStream() != end.getStream()) {
        return;
    }
    data.insert(data.begin() + pos.GetOffset(), start.getStream()->data.begin() + start.GetOffset(),
                end.getStream()->data.begin() + end.GetOffset());
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
DataBlock::DataBlock(std::vector<T> *vec) {
    SetWordSize(sizeof(T));
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

#endif  // SRC_GENIE_UTIL_DATA_BLOCK_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
