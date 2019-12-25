/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "mpegg-raw-au.h"
#include <util/make_unique.h>
#include <algorithm>

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor::SubDescriptor(size_t wordsize, GenomicSubsequence _id)
    : data(0, wordsize), position(0), id(_id) {}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor::SubDescriptor(gabac::DataBlock *d, GenomicSubsequence _id)
    : data(0, 1), position(0), id(_id) {
    data.swap(d);
}

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::SubDescriptor::push(uint64_t val) { data.push_back(val); }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::SubDescriptor::inc() { position++; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t MpeggRawAu::SubDescriptor::get() const { return data.get(position); }

// ---------------------------------------------------------------------------------------------------------------------

bool MpeggRawAu::SubDescriptor::end() const { return data.size() == position; }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::SubDescriptor::swap(gabac::DataBlock *block) { block->swap(&data); }

// ---------------------------------------------------------------------------------------------------------------------

GenomicSubsequence MpeggRawAu::SubDescriptor::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::unique_ptr<MpeggRawAu::SubDescriptor>> &MpeggRawAu::Descriptor::getSubsequences() const {
    return subdesc;
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor *MpeggRawAu::Descriptor::getSubsequence(GenomicSubsequence sub) {
    return subdesc[uint8_t(sub)].get();
}

// ---------------------------------------------------------------------------------------------------------------------

GenomicDescriptor MpeggRawAu::Descriptor::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::Descriptor::add(std::unique_ptr<SubDescriptor> sub) { subdesc.push_back(std::move(sub)); }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::Descriptor::set(GenomicSubsequence _id, std::unique_ptr<SubDescriptor> sub) {
    subdesc[uint8_t(_id)] = std::move(sub);
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::Descriptor::Descriptor(GenomicDescriptor _id) : id(_id) {}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor &MpeggRawAu::get(GenomicDescriptor desc, GenomicSubsequence sub) {
    return *descriptors[uint8_t(desc)]->getSubsequence(sub);
}

// ---------------------------------------------------------------------------------------------------------------------

const MpeggRawAu::SubDescriptor &MpeggRawAu::get(GenomicDescriptor desc, GenomicSubsequence sub) const {
    return *descriptors[uint8_t(desc)]->getSubsequence(sub);
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor &MpeggRawAu::get(GenomicDescriptor desc, uint8_t sub) {
    return get(desc, GenomicSubsequence(sub));
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::unique_ptr<MpeggRawAu::Descriptor>> &MpeggRawAu::getDescriptorStreams() const {
    return descriptors;
}

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::set(GenomicDescriptor desc, GenomicSubsequence sub, std::unique_ptr<SubDescriptor> data) {
    descriptors[uint8_t(desc)]->set(sub, std::move(data));
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::MpeggRawAu(std::unique_ptr<format::mpegg_p2::ParameterSet> set, size_t _numRecords)
    : descriptors(0), parameters(std::move(set)), numRecords(_numRecords) {
    const size_t WORDSIZE = 4;
    for (const auto &desc : getDescriptors()) {
        auto desc_data = util::make_unique<Descriptor>(desc.id);
        for (const auto &subdesc : desc.subseqs) {
            desc_data->add(util::make_unique<SubDescriptor>(WORDSIZE, subdesc.id));
        }
        descriptors.push_back(std::move(desc_data));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::setParameters(std::unique_ptr<format::mpegg_p2::ParameterSet> _parameters) {
    parameters = std::move(_parameters);
}

// ---------------------------------------------------------------------------------------------------------------------

const format::mpegg_p2::ParameterSet *MpeggRawAu::getParameters() const { return parameters.get(); }

// ---------------------------------------------------------------------------------------------------------------------

format::mpegg_p2::ParameterSet *MpeggRawAu::getParameters() { return parameters.get(); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<format::mpegg_p2::ParameterSet> MpeggRawAu::moveParameters() { return std::move(parameters); }

// ---------------------------------------------------------------------------------------------------------------------

size_t MpeggRawAu::getNumRecords() const { return numRecords; }

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------