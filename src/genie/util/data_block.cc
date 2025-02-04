/**
 * Copyright 2018-2024 The Genie Authors.
 * @file data_block.cc
 * @brief Implementation of the DataBlock class for managing blocks of data.
 *
 * This file contains the implementation of the DataBlock class, which provides
 * mechanisms for managing and manipulating blocks of data. It includes methods
 * for resizing, clearing, reserving space, and comparing data blocks, as well
 * as constructors for initializing data blocks from various sources.
 *
 * @details The DataBlock class supports operations such as getting a reader
 * for the data block, checking equality between data blocks, and swapping data
 * between blocks. It also provides constructors for creating data blocks from
 * sizes, vectors, and strings.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#include "genie/util/data_block.h"

#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

BlockStepper DataBlock::GetReader() const {
  return BlockStepper(const_cast<uint8_t*>(data_.data()),
                      const_cast<uint8_t*>(data_.data() + data_.size()),
                      GetWordSize());
  // TODO(Fabian): Add BlockStepper for const
}

// -----------------------------------------------------------------------------

bool DataBlock::operator==(const DataBlock& d) const {
  return lg_word_size_ == d.lg_word_size_ && data_ == d.data_;
}

// -----------------------------------------------------------------------------

DataBlock& DataBlock::operator=(const std::initializer_list<uint64_t>& il) {
  Resize(il.size());
  size_t ctr = 0;
  for (const auto& v : il) {
    Set(ctr, v);
    ++ctr;
  }
  return *this;
}

// -----------------------------------------------------------------------------

size_t DataBlock::Size() const { return DivByWordSize(data_.size()); }

// -----------------------------------------------------------------------------

[[maybe_unused]] void DataBlock::Reserve(const size_t size) {
  data_.reserve(MulByWordSize(size));
}

// -----------------------------------------------------------------------------

void DataBlock::Clear() { data_.clear(); }

// -----------------------------------------------------------------------------

void DataBlock::Resize(const size_t size) { data_.resize(MulByWordSize(size)); }

// -----------------------------------------------------------------------------

bool DataBlock::Empty() const { return data_.empty(); }

// -----------------------------------------------------------------------------

void DataBlock::Swap(DataBlock* const d) {
  const size_t tmp = lg_word_size_;
  lg_word_size_ = d->lg_word_size_;
  d->lg_word_size_ = static_cast<uint8_t>(tmp);
  data_.swap(d->data_);
}

// -----------------------------------------------------------------------------

DataBlock::DataBlock(const size_t size, const uint8_t word_size)
    : lg_word_size_(0) {
  SetWordSize(word_size);
  data_.resize(size * word_size);
}

// -----------------------------------------------------------------------------

DataBlock::DataBlock(std::vector<uint8_t>* vec) : lg_word_size_(0) {
  this->data_.swap(*vec);
}

// -----------------------------------------------------------------------------

DataBlock::DataBlock(std::string* vec) : lg_word_size_(0) {
  const size_t size = vec->size() * sizeof(char);
  this->data_.resize(size);
  this->data_.shrink_to_fit();
  std::memcpy(this->data_.data(), vec->data(), size);
  vec->clear();
}

// -----------------------------------------------------------------------------

DataBlock::DataBlock(const uint8_t* d, const size_t size,
                     const uint8_t word_size)
    : lg_word_size_(0) {
  SetWordSize(word_size);
  const size_t s = size * word_size;
  this->data_.resize(s);
  this->data_.shrink_to_fit();
  std::memcpy(this->data_.data(), d, s);
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
