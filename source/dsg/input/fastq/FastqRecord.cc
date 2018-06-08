// Copyright 2018 The genie authors


/**
 *  @file FastqRecord.cc
 *  @brief FASTQ record implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "input/fasta/FastqRecord.h"

#include <iostream>


namespace dsg {


FastqRecord::FastqRecord(
    const std::string& title,
    const std::string& sequence,
    const std::string& optional,
    const std::string& qualityScores)
    : title(title),
      sequence(sequence),
      optional(optional),
      qualityScores(qualityScores)
{
    // Nothing to do here.
}


FastqRecord::~FastqRecord(void)
{
    // Nothing to do here.
}


}  // namespace dsg

