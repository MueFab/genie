// Copyright 2018 The genie authors


/**
 *  @file FastaRecord.cc
 *  @brief FASTA record implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "FastaRecord.h"

#include <iostream>


namespace dsg {
namespace input {
namespace fasta {


FastaRecord::FastaRecord(
    const std::string& header,
    const std::string& sequence)
    : header(header),
      sequence(sequence)
{
    // Nothing to do here.
}


FastaRecord::~FastaRecord(void)
{
    // Nothing to do here.
}


}  // namespace fasta
}  // namespace input
}  // namespace dsg

