// Copyright 2018 The genie authors


/**
 *  @file FastaIndexEntry.cc
 *  @brief FASTA index entry implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "input/fasta/FastaIndexEntry.h"

#include "common/log.h"


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
        DSG_LOG("Warning: FASTA index entry with empty header was added.\n");
    }

    if (this->length == 0) {
        DSG_LOG("Warning: FASTA index entry with length=0 was added.\n");
    }
}

FastaIndexEntry::~FastaIndexEntry(void)
{
    // Nothing to do here.
}


}  // namespace dsg
