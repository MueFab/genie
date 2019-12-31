/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "mpegg-raw-au.h"
#include <util/make_unique.h>
#include <algorithm>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor::SubDescriptor(size_t wordsize, GenSubIndex _id) : data(0, wordsize), position(0), id(std::move(_id)) {}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor::SubDescriptor(gabac::DataBlock d, GenSubIndex _id)
    : data(std::move(d)), position(0), id(std::move(_id)) {}

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::SubDescriptor::push(uint64_t val) { data.push_back(val); }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::SubDescriptor::inc() { position++; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t MpeggRawAu::SubDescriptor::get() const { return data.get(position); }

// ---------------------------------------------------------------------------------------------------------------------

bool MpeggRawAu::SubDescriptor::end() const { return data.size() == position; }

// ---------------------------------------------------------------------------------------------------------------------

gabac::DataBlock &&MpeggRawAu::SubDescriptor::move() { return std::move(data); }

// ---------------------------------------------------------------------------------------------------------------------

GenSubIndex MpeggRawAu::SubDescriptor::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<MpeggRawAu::SubDescriptor> &MpeggRawAu::Descriptor::getSubsequences() const { return subdesc; }

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor &MpeggRawAu::Descriptor::getSubsequence(uint8_t sub) { return subdesc[uint8_t(sub)]; }

// ---------------------------------------------------------------------------------------------------------------------

GenDesc MpeggRawAu::Descriptor::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::Descriptor::add(SubDescriptor&& sub) { subdesc.push_back(std::move(sub)); }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::Descriptor::set(uint8_t _id, SubDescriptor&& sub) { subdesc[uint8_t(_id)] = std::move(sub); }

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::Descriptor::Descriptor(GenDesc _id) : id(_id) {}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor &MpeggRawAu::get(GenSubIndex sub) {
    return descriptors[uint8_t(sub.first)].getSubsequence(sub.second);
}

// ---------------------------------------------------------------------------------------------------------------------

const MpeggRawAu::SubDescriptor &MpeggRawAu::get(GenSubIndex sub) const {
    return descriptors[uint8_t(sub.first)].getSubsequences()[sub.second];
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::SubDescriptor &MpeggRawAu::get(GenDesc desc, uint8_t sub) { return get(GenSubIndex(desc, sub)); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<MpeggRawAu::Descriptor> &MpeggRawAu::getDescriptorStreams() const { return descriptors; }

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::set(GenSubIndex sub, SubDescriptor&& data) {
    descriptors[uint8_t(sub.first)].set(sub.second, std::move(data));
}

// ---------------------------------------------------------------------------------------------------------------------

MpeggRawAu::MpeggRawAu(format::mpegg_p2::ParameterSet&& set, size_t _numRecords)
    : descriptors(), parameters(std::move(set)), numRecords(_numRecords) {
    const size_t WORDSIZE = 4;
    for (const auto &desc : getDescriptors()) {
        Descriptor desc_data(desc.id);
        for (const auto &subdesc : desc.subseqs) {
            desc_data.add(SubDescriptor(WORDSIZE, subdesc.id));
        }
        descriptors.push_back(std::move(desc_data));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MpeggRawAu::setParameters(format::mpegg_p2::ParameterSet&& _parameters) { parameters = std::move(_parameters); }

// ---------------------------------------------------------------------------------------------------------------------

const format::mpegg_p2::ParameterSet &MpeggRawAu::getParameters() const { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

format::mpegg_p2::ParameterSet &MpeggRawAu::getParameters() { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

format::mpegg_p2::ParameterSet &&MpeggRawAu::moveParameters() { return std::move(parameters); }

// ---------------------------------------------------------------------------------------------------------------------

size_t MpeggRawAu::getNumRecords() const { return numRecords; }

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------