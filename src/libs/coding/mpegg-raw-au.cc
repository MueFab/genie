/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "mpegg-raw-au.h"

#include <util/make_unique.h>
#include <algorithm>

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor::SubDescriptor(size_t wordsize) : data(0, wordsize), position(0) {}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor::SubDescriptor(gabac::DataBlock *d) : data(0, 1), position(0) { d->swap(d); }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::SubDescriptor::push(uint64_t val) { data.push_back(val); }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::SubDescriptor::inc() { position++; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t MpeggRawAu::SubDescriptor::get() const { return data.get(position); }

// ---------------------------------------------------------------------------------------------------------------------

bool MpeggRawAu::SubDescriptor::end() const { return data.size() == position; }

// ---------------------------------------------------------------------------------------------------------------------

size_t MpeggRawAu::SubDescriptor::rawSize() const { return data.getRawSize(); }

// ---------------------------------------------------------------------------------------------------------------------

size_t MpeggRawAu::SubDescriptor::getWordSize() const { return data.getWordSize(); }

// ---------------------------------------------------------------------------------------------------------------------

const void *MpeggRawAu::SubDescriptor::getData() const { return data.getData(); }

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor &MpeggRawAu::get(GenomicDescriptor desc, GenomicSubsequence sub) {
    return descriptors[uint8_t(desc)][uint8_t (sub)];
}

// ---------------------------------------------------------------------------------------------------------------------

const MpeggRawAu::SubDescriptor &MpeggRawAu::get(GenomicDescriptor desc, GenomicSubsequence sub) const {
    return descriptors[uint8_t(desc)][uint8_t(sub)];
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::MpeggRawAu(std::unique_ptr<format::mpegg_p2::ParameterSet> set)
    : descriptors(getDescriptors().size()), parameters(std::move(set)) {
    const size_t WORDSIZE = 4;
    size_t idx = 0;
    for (const auto &s : getDescriptors()) {
        descriptors[idx].resize(s.subseqs.size(), SubDescriptor(WORDSIZE));
        idx++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------