/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <limits>

#include "data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

BlockStepper util::DataBlock::getReader() const {
    return BlockStepper(const_cast<uint8_t *>(data.data()), const_cast<uint8_t *>(data.data() + data.size()),
                        static_cast<uint8_t>(getWordSize()));
    // TODO(Fabian): Add BlockStepper for const
}

// ---------------------------------------------------------------------------------------------------------------------

bool util::DataBlock::operator==(const util::DataBlock &d) const {
    return lgWordSize == d.lgWordSize && data == d.data;
}

// ---------------------------------------------------------------------------------------------------------------------

DataBlock &DataBlock::operator=(const std::initializer_list<uint64_t> &il) {
    resize(il.size());
    size_t ctr = 0;
    for (const auto &v : il) {
        set(ctr, v);
        ++ctr;
    }
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

size_t util::DataBlock::size() const { return divByWordSize(data.size()); }

// ---------------------------------------------------------------------------------------------------------------------

void util::DataBlock::reserve(size_t size) { data.reserve(mulByWordSize(size)); }

// ---------------------------------------------------------------------------------------------------------------------

void util::DataBlock::shrink_to_fit() { data.shrink_to_fit(); }

// ---------------------------------------------------------------------------------------------------------------------

void util::DataBlock::clear() { data.clear(); }

// ---------------------------------------------------------------------------------------------------------------------

void util::DataBlock::resize(size_t size) { data.resize(mulByWordSize(size)); }

// ---------------------------------------------------------------------------------------------------------------------

bool util::DataBlock::empty() const { return data.empty(); }

// ---------------------------------------------------------------------------------------------------------------------

void util::DataBlock::swap(util::DataBlock *const d) {
    size_t tmp = lgWordSize;
    lgWordSize = d->lgWordSize;
    d->lgWordSize = static_cast<uint8_t>(tmp);
    data.swap(d->data);
}

// ---------------------------------------------------------------------------------------------------------------------

DataBlock::DataBlock(size_t size, size_t wsize) {
    setWordSize(wsize);
    data.resize(size * wsize);
}

// ---------------------------------------------------------------------------------------------------------------------

DataBlock::DataBlock(std::vector<uint8_t> *vec) : lgWordSize(1) { this->data.swap(*vec); }

// ---------------------------------------------------------------------------------------------------------------------

DataBlock::DataBlock(std::string *vec) : lgWordSize(1) {
    size_t size = vec->size() * sizeof(char);
    this->data.resize(size);
    this->data.shrink_to_fit();
    std::memcpy(this->data.data(), vec->data(), size);
    vec->clear();
}

// ---------------------------------------------------------------------------------------------------------------------

DataBlock::DataBlock(const uint8_t *d, size_t size, uint8_t word_size) {
    setWordSize(word_size);
    size_t s = size * word_size;
    this->data.resize(s);
    this->data.shrink_to_fit();
    std::memcpy(this->data.data(), d, s);
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t util::DataBlock::getMaximum() const {
    BlockStepper r = getReader();
    uint64_t max = std::numeric_limits<uint64_t>::min();
    while (r.isValid()) {
        uint64_t symbol = r.get();
        max = std::max(max, symbol);
        r.inc();
    }
    return max;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t util::DataBlock::getMaxWordSize() const {
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

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
