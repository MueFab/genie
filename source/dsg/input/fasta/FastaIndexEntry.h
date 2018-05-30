// Copyright 2018 The genie authors


/**
 *  @file FastaIndexEntry.h
 *  @brief FASTA index entry interface
 *  @author Jan Voges
 *  @bug No known bugs
 */


#ifndef DSG_INPUT_FASTA_FASTAINDEXENTRY_H_
#define DSG_INPUT_FASTA_FASTAINDEXENTRY_H_


#include <string>


namespace dsg {


class FastaIndexEntry {
 public:

    FastaIndexEntry(
        const std::string& header,
        const size_t offset,
        const size_t sequenceLength);

    ~FastaIndexEntry(void);

public:

    std::string header;

    size_t offset;

    size_t length;
};


}  // namespace dsg


#endif  // DSG_INPUT_FASTA_FASTAINDEXENTRY_H_
