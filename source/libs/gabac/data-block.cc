/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "data-block.h"

#include <limits>
#include <utility>

#include "block-stepper.h"

namespace gabac {

BlockStepper DataBlock::getReader() const {
    return {const_cast<uint8_t *>(data.data()),
            const_cast<uint8_t *>(data.end().base()),
            wordSize};  // TODO(Fabian): Add BlockStepper for const
}

bool DataBlock::operator==(const DataBlock &d) const {
    return wordSize == d.wordSize && data == d.data;
}

DataBlock &DataBlock::operator=(const std::initializer_list<uint64_t> &il) {
    resize(il.size());
    size_t ctr = 0;
    for (const auto &v : il) {
        set(ctr, v);
        ++ctr;
    }
    return *this;
}

size_t DataBlock::size() const { return data.size() / wordSize; }

void DataBlock::reserve(size_t size) { data.reserve(size * wordSize); }

void DataBlock::shrink_to_fit() { data.shrink_to_fit(); }

void DataBlock::clear() { data.clear(); }

void DataBlock::resize(size_t size) { data.resize(size * wordSize); }

bool DataBlock::empty() const { return data.empty(); }

void DataBlock::swap(DataBlock *const d) {
    size_t tmp = wordSize;
    wordSize = d->wordSize;
    d->wordSize = static_cast<uint8_t>(tmp);
    data.swap(d->data);
}

DataBlock::DataBlock(size_t size, size_t wsize)
    : wordSize(static_cast<uint8_t>(wsize)), data(size * wsize) {}

DataBlock::DataBlock(std::vector<uint8_t> *vec) : wordSize(1) {
    this->data.swap(*vec);
}

DataBlock::DataBlock(std::string *vec) : wordSize(1) {
    size_t size = vec->size() * sizeof(char);
    this->data.resize(size);
    this->data.shrink_to_fit();
    std::memcpy(this->data.data(), vec->data(), size);
    vec->clear();
}

DataBlock::DataBlock(const uint8_t *d, size_t size, uint8_t word_size)
    : wordSize(word_size) {
    size_t s = size * word_size;
    this->data.resize(s);
    this->data.shrink_to_fit();
    std::memcpy(this->data.data(), d, s);
}

uint64_t DataBlock::getMaximum() const {
    BlockStepper r = getReader();
    uint64_t max = std::numeric_limits<uint64_t>::min();
    while (r.isValid()) {
        uint64_t symbol = r.get();
        max = std::max(max, symbol);
        r.inc();
    }
    return max;
}

uint8_t DataBlock::getMaxWordSize() const {
    uint8_t w = 1;
    while (w != 8) {
        w *= 2;
        if (getRawSize() % w) {
            w /= 2;
            break;
        }
    }
    return w;
}

}  // namespace gabac
