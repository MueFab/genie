/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "access-unit-raw.h"
#include <genie/util/make-unique.h>
#include <algorithm>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitRaw::Subsequence::Subsequence(size_t wordsize, GenSubIndex _id)
    : data(0, wordsize), position(0), id(std::move(_id)) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitRaw::Subsequence::Subsequence(util::DataBlock d, GenSubIndex _id)
    : data(std::move(d)), position(0), id(std::move(_id)) {}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::Subsequence::push(uint64_t val) { data.push_back(val); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::Subsequence::inc() { position++; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnitRaw::Subsequence::get(size_t lookahead) const { return data.get(position + lookahead); }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnitRaw::Subsequence::end() const { return data.size() <= position; }

// ---------------------------------------------------------------------------------------------------------------------

util::DataBlock &&AccessUnitRaw::Subsequence::move() { return std::move(data); }

// ---------------------------------------------------------------------------------------------------------------------

GenSubIndex AccessUnitRaw::Subsequence::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnitRaw::Subsequence::getNumSymbols() const { return data.size(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnitRaw::Subsequence::pull() {
    UTILS_DIE_IF(end(), "Tried to read descriptor that has already ended");
    return data.get(position++);
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitRaw::Subsequence &AccessUnitRaw::Descriptor::get(uint16_t sub) { return subdesc[sub]; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnitRaw::Subsequence &AccessUnitRaw::Descriptor::get(uint16_t sub) const { return subdesc[sub]; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitRaw::Subsequence &AccessUnitRaw::Descriptor::getTokenType(uint16_t pos, uint8_t type) {
    uint16_t s_id = ((pos << 4u) | (type & 0xfu));
    while (subdesc.size() <= s_id) {
        subdesc.emplace_back(4, GenSubIndex(getID(), subdesc.size()));
    }
    return get(s_id);
}

// ---------------------------------------------------------------------------------------------------------------------

GenDesc AccessUnitRaw::Descriptor::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::Descriptor::add(Subsequence &&sub) { subdesc.push_back(std::move(sub)); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::Descriptor::set(uint16_t _id, Subsequence &&sub) { subdesc[uint16_t(_id)] = std::move(sub); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitRaw::Descriptor::Descriptor(GenDesc _id) : id(_id) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitRaw::Subsequence *AccessUnitRaw::Descriptor::begin() { return &subdesc.front(); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitRaw::Subsequence *AccessUnitRaw::Descriptor::end() { return &subdesc.back() + 1; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnitRaw::Subsequence *AccessUnitRaw::Descriptor::begin() const { return &subdesc.front(); }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnitRaw::Subsequence *AccessUnitRaw::Descriptor::end() const { return &subdesc.back() + 1; }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnitRaw::Descriptor::getSize() const { return subdesc.size(); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitRaw::Subsequence &AccessUnitRaw::get(GenSubIndex sub) {
    return descriptors[uint8_t(sub.first)].get(sub.second);
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitRaw::Descriptor &AccessUnitRaw::get(GenDesc desc) { return descriptors[uint8_t(desc)]; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnitRaw::Descriptor &AccessUnitRaw::get(GenDesc desc) const { return descriptors[uint8_t(desc)]; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnitRaw::Subsequence &AccessUnitRaw::get(GenSubIndex sub) const {
    return descriptors[uint8_t(sub.first)].get(sub.second);
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::set(GenSubIndex sub, Subsequence &&data) {
    descriptors[uint8_t(sub.first)].set(sub.second, std::move(data));
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::push(GenSubIndex sub, uint64_t value) { get(sub).push(value); }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnitRaw::isEnd(GenSubIndex sub) { return get(sub).end(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnitRaw::peek(GenSubIndex sub, size_t lookahead) { return get(sub).get(lookahead); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnitRaw::pull(GenSubIndex sub) { return get(sub).pull(); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitRaw::AccessUnitRaw(parameter::ParameterSet &&set, size_t _numRecords)
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

void AccessUnitRaw::setParameters(parameter::ParameterSet &&_parameters) { parameters = std::move(_parameters); }

// ---------------------------------------------------------------------------------------------------------------------

const parameter::ParameterSet &AccessUnitRaw::getParameters() const { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

parameter::ParameterSet &AccessUnitRaw::getParameters() { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

parameter::ParameterSet &&AccessUnitRaw::moveParameters() { return std::move(parameters); }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnitRaw::getNumRecords() const { return numRecords; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::clear() { *this = AccessUnitRaw(parameter::ParameterSet(), 0); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::addRecord() { numRecords++; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::setReference(uint16_t ref) { referenceSequence = ref; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t AccessUnitRaw::getReference() { return referenceSequence; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::setMaxPos(uint64_t pos) { maxPos = pos; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::setMinPos(uint64_t pos) { minPos = pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnitRaw::getMaxPos() const { return maxPos; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnitRaw::getMinPos() const { return minPos; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::setNumRecords(size_t recs) { numRecords = recs; }

// ---------------------------------------------------------------------------------------------------------------------

record::ClassType AccessUnitRaw::getClassType() const { return type; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitRaw::setClassType(record::ClassType _type) { type = _type; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitRaw::Descriptor *AccessUnitRaw::begin() { return &descriptors.front(); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitRaw::Descriptor *AccessUnitRaw::end() { return &descriptors.back() + 1; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnitRaw::Descriptor *AccessUnitRaw::begin() const { return &descriptors.front(); }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnitRaw::Descriptor *AccessUnitRaw::end() const { return &descriptors.back() + 1; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------