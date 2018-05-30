/** @file SAMBlock.cc
 *  @brief This file contains the implementation of the SAMBlock class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#include "IO/SAM/SAMBlock.h"

namespace calq {

SAMBlock::SAMBlock(void)
    : records(),
      nrMappedRecords_(0),
      nrUnmappedRecords_(0) {}

SAMBlock::~SAMBlock(void) {}

size_t SAMBlock::nrMappedRecords(void) const {
    return nrMappedRecords_;
}

size_t SAMBlock::nrUnmappedRecords(void) const {
    return nrUnmappedRecords_;
}

size_t SAMBlock::nrRecords(void) const {
    return (nrMappedRecords_ + nrUnmappedRecords_);
}

void SAMBlock::reset(void) {
    records.clear();
    nrMappedRecords_ = 0;
    nrUnmappedRecords_ = 0;
}

}  // namespace calq

