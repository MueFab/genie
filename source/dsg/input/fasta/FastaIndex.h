// Copyright 2018 The genie authors


/**
 *  @file FastaIndex.h
 *  @brief FASTA index interface
 *  @author Jan Voges
 *  @bug No known bugs
 */

#ifndef DSG_INPUT_FASTA_FASTAINDEX_H_
#define DSG_INPUT_FASTA_FASTAINDEX_H_


#include <vector>

#include "input/fasta/FastaIndexEntry.h"


namespace dsg {

class FastaIndex {
 public:

    FastaIndex(void);

    ~FastaIndex(void);

    void addEntry(const FASTAIndexEntry& entry);

    size_t size(void) const;

 public:

    std::vector<FastaIndexEntry> entries;
};


}  // namespace dsg


#endif  // DSG_INPUT_FASTA_FASTAINDEX_H_
