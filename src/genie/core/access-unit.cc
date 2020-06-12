/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "access-unit.h"
#include <genie/util/make-unique.h>
#include <algorithm>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(size_t wordsize, GenSubIndex _id)
    : data(0, wordsize), position(0), id(std::move(_id)) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(util::DataBlock d, GenSubIndex _id)
    : data(std::move(d)), position(0), id(std::move(_id)) {}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::push(uint64_t val) { data.push_back(val); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::inc() { position++; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::Subsequence::get(size_t lookahead) const { return data.get(position + lookahead); }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::Subsequence::end() const { return data.size() <= position; }

// ---------------------------------------------------------------------------------------------------------------------

util::DataBlock &&AccessUnit::Subsequence::move() { return std::move(data); }

// ---------------------------------------------------------------------------------------------------------------------

GenSubIndex AccessUnit::Subsequence::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnit::Subsequence::getNumSymbols() const { return data.size(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::Subsequence::pull() {
    UTILS_DIE_IF(end(), "Tried to read descriptor that has already ended");
    return data.get(position++);
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence &AccessUnit::Descriptor::get(uint16_t sub) { return subdesc[sub]; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Subsequence &AccessUnit::Descriptor::get(uint16_t sub) const { return subdesc[sub]; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence &AccessUnit::Descriptor::getTokenType(uint16_t pos, uint8_t type) {
    uint16_t s_id = ((pos << 4u) | (type & 0xfu));
    while (subdesc.size() <= s_id) {
        subdesc.emplace_back(4, GenSubIndex(getID(), subdesc.size()));
    }
    return get(s_id);
}

// ---------------------------------------------------------------------------------------------------------------------

GenDesc AccessUnit::Descriptor::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Descriptor::add(Subsequence &&sub) { subdesc.push_back(std::move(sub)); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Descriptor::set(uint16_t _id, Subsequence &&sub) { subdesc[uint16_t(_id)] = std::move(sub); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Descriptor::Descriptor(GenDesc _id) : id(_id) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence *AccessUnit::Descriptor::begin() { return &subdesc.front(); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence *AccessUnit::Descriptor::end() { return &subdesc.back() + 1; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Subsequence *AccessUnit::Descriptor::begin() const { return &subdesc.front(); }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Subsequence *AccessUnit::Descriptor::end() const { return &subdesc.back() + 1; }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnit::Descriptor::getSize() const { return subdesc.size(); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence &AccessUnit::get(GenSubIndex sub) {
    return descriptors[uint8_t(sub.first)].get(sub.second);
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Descriptor &AccessUnit::get(GenDesc desc) { return descriptors[uint8_t(desc)]; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Descriptor &AccessUnit::get(GenDesc desc) const { return descriptors[uint8_t(desc)]; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Subsequence &AccessUnit::get(GenSubIndex sub) const {
    return descriptors[uint8_t(sub.first)].get(sub.second);
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::set(GenSubIndex sub, Subsequence &&data) {
    descriptors[uint8_t(sub.first)].set(sub.second, std::move(data));
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::push(GenSubIndex sub, uint64_t value) { get(sub).push(value); }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::isEnd(GenSubIndex sub) { return get(sub).end(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::peek(GenSubIndex sub, size_t lookahead) { return get(sub).get(lookahead); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::pull(GenSubIndex sub) { return get(sub).pull(); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(parameter::ParameterSet &&set, size_t _numRecords)
    : descriptors(), parameters(std::move(set)), numRecords(_numRecords), minPos(0), maxPos(0), referenceSequence(0) {
    const size_t WORDSIZE = 4;
    for (const auto &desc : getDescriptors()) {
        Descriptor desc_data(desc.id);
        for (const auto &subdesc : desc.subseqs) {
            desc_data.add(Subsequence(WORDSIZE, subdesc.id));
        }
        descriptors.push_back(std::move(desc_data));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setParameters(parameter::ParameterSet &&_parameters) { parameters = std::move(_parameters); }

// ---------------------------------------------------------------------------------------------------------------------

const parameter::ParameterSet &AccessUnit::getParameters() const { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

parameter::ParameterSet &AccessUnit::getParameters() { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

parameter::ParameterSet &&AccessUnit::moveParameters() { return std::move(parameters); }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnit::getNumRecords() const { return numRecords; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::clear() { *this = AccessUnit(parameter::ParameterSet(), 0); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::addRecord() { numRecords++; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setReference(uint16_t ref) { referenceSequence = ref; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t AccessUnit::getReference() { return referenceSequence; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setMaxPos(uint64_t pos) { maxPos = pos; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setMinPos(uint64_t pos) { minPos = pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::getMaxPos() const { return maxPos; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::getMinPos() const { return minPos; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setNumRecords(size_t recs) { numRecords = recs; }

// ---------------------------------------------------------------------------------------------------------------------

record::ClassType AccessUnit::getClassType() const { return type; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setClassType(record::ClassType _type) { type = _type; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Descriptor *AccessUnit::begin() { return &descriptors.front(); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Descriptor *AccessUnit::end() { return &descriptors.back() + 1; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Descriptor *AccessUnit::begin() const { return &descriptors.front(); }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Descriptor *AccessUnit::end() const { return &descriptors.back() + 1; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------