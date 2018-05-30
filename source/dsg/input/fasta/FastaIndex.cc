// Copyright 2018 The genie authors


/**
 *  @file FastaIndex.cc
 *  @brief FASTA index implementation
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "input/fasta/FastaIndex.h"

#include "common/log.h"


namespace dsg {


FastaIndex::FastaIndex(void)
    : entries()
{
    // Nothing to do here.
}

FastaIndex::~FastaIndex(void)
{
    // Nothing to do here.
}

void FastaIndex::addEntry(const FASTAIndexEntry& entry)
{
    entries.push_back(entry);
}


void FastaIndex::print(FILE *stream)
{
    fprintf(stream, "FASTA index with %lu entries:\n", size());
    fprintf(stream, "header | offset | length\n");

    for (const auto& entry : entries) {
        fprintf(stream, "%s | ", entry.header.c_str());
        fprintf(stream, "%lu | ", entry.offset);
        fprintf(stream, "%lu | ", entry.length);
    }
}


size_t FastaIndex::size(void) const
{
    return entries.size();
}


}  // namespace dsg

