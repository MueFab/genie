/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/util/data-block.h"
#include <algorithm>
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

BlockStepper DataBlock::getReader() const {
    return BlockStepper(const_cast<uint8_t *>(data.data()), const_cast<uint8_t *>(data.data() + data.size()),
                        getWordSize());
    // TODO(Fabian): Add BlockStepper for const
}

// ---------------------------------------------------------------------------------------------------------------------

bool DataBlock::operator==(const DataBlock &d) const {
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

size_t DataBlock::size() const { return divByWordSize(data.size()); }

// ---------------------------------------------------------------------------------------------------------------------

void DataBlock::reserve(size_t size) { data.reserve(mulByWordSize(size)); }

// ---------------------------------------------------------------------------------------------------------------------

void DataBlock::clear() { data.clear(); }

// ---------------------------------------------------------------------------------------------------------------------

void DataBlock::resize(size_t size) { data.resize(mulByWordSize(size)); }

// ---------------------------------------------------------------------------------------------------------------------

bool DataBlock::empty() const { return data.empty(); }

// ---------------------------------------------------------------------------------------------------------------------

void DataBlock::swap(DataBlock *const d) {
    size_t tmp = lgWordSize;
    lgWordSize = d->lgWordSize;
    d->lgWordSize = static_cast<uint8_t>(tmp);
    data.swap(d->data);
}

// ---------------------------------------------------------------------------------------------------------------------

DataBlock::DataBlock(size_t size, uint8_t word_size) : lgWordSize(0) {
    setWordSize(word_size);
    data.resize(size * word_size);
}

// ---------------------------------------------------------------------------------------------------------------------

DataBlock::DataBlock(std::vector<uint8_t> *vec) : lgWordSize(0) { this->data.swap(*vec); }

// ---------------------------------------------------------------------------------------------------------------------

DataBlock::DataBlock(std::string *vec) : lgWordSize(0) {
    size_t size = vec->size() * sizeof(char);
    this->data.resize(size);
    this->data.shrink_to_fit();
    std::memcpy(this->data.data(), vec->data(), size);
    vec->clear();
}

// ---------------------------------------------------------------------------------------------------------------------

DataBlock::DataBlock(const uint8_t *d, size_t size, uint8_t word_size) : lgWordSize(0) {
    setWordSize(word_size);
    size_t s = size * word_size;
    this->data.resize(s);
    this->data.shrink_to_fit();
    std::memcpy(this->data.data(), d, s);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
