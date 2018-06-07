// Copyright 2018 The genie authors


/**
 *  @file FastaIndexEntry.cc
 *  @brief FASTA index entry implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "input/fasta/FastaIndexEntry.h"

#include <iostream>


namespace dsg {


FastaIndexEntry::FastaIndexEntry(
    const std::string& header,
    const size_t offset,
    const size_t length)
    : this->header(header),
      this->offset(offset),
      this->length(length)
{
    if (this->header.empty() == true) {
        std::cout << "Warning: FASTA index entry with empty header was added." << std::endl;
    }

    if (this->length == 0) {
        std::cout << "Warning: FASTA index entry with length=0 was added." << std::endl;
    }
}

FastaIndexEntry::~FastaIndexEntry(void)
{
    // Nothing to do here.
}


}  // namespace dsg
