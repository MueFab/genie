/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/access-unit.h"
#include <algorithm>
#include <utility>
#include <vector>
#include "genie/core/mismatch-decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence &AccessUnit::Subsequence::operator=(const Subsequence &sub) {
    data = sub.data;
    position = sub.position;
    id = sub.id;
    numSymbols = sub.numSymbols;
    dependency = sub.dependency;
    if (sub.mmDecoder != nullptr) mmDecoder = sub.mmDecoder->copy();
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence &AccessUnit::Subsequence::operator=(AccessUnit::Subsequence &&sub) noexcept {
    data = std::move(sub.data);
    position = sub.position;
    id = sub.id;
    numSymbols = sub.numSymbols;
    dependency = sub.dependency;
    mmDecoder = std::move(sub.mmDecoder);
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(const Subsequence &sub) { *this = sub; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(Subsequence &&sub) noexcept { *this = std::move(sub); }

// ---------------------------------------------------------------------------------------------------------------------

util::DataBlock *AccessUnit::Subsequence::getDependency() {
    if (id == GenSub::MMTYPE_SUBSTITUTION || id == GenSub::RFTT)
        return &dependency;
    else
        return nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit::Subsequence AccessUnit::Subsequence::attachMismatchDecoder(std::unique_ptr<MismatchDecoder> mm) {
    mmDecoder = std::move(mm);
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

MismatchDecoder *AccessUnit::Subsequence::getMismatchDecoder() const { return mmDecoder.get(); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(uint8_t wordSize, GenSubIndex _id)
    : data(0, wordSize), position(0), id(std::move(_id)), dependency(0, wordSize) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(util::DataBlock d, GenSubIndex _id)
    : data(std::move(d)), position(0), id(std::move(_id)), dependency(0, d.getWordSize()) {}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::push(uint64_t val) { data.push_back(val); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::pushDependency(uint64_t val) { dependency.push_back(val); }

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
    if (end()) {
        UTILS_DIE("Tried to read descriptor that has already ended: " +
                  std::to_string(static_cast<uint8_t>(this->getID().first)) + ", " +
                  std::to_string(this->getID().second));
    }
    return data.get(position++);
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence &AccessUnit::Descriptor::get(uint16_t sub) { return subdesc[sub]; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Subsequence &AccessUnit::Descriptor::get(uint16_t sub) const { return subdesc[sub]; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence &AccessUnit::Descriptor::getTokenType(uint16_t pos, uint8_t _type) {
    uint16_t s_id = ((pos << 4u) | (_type & 0xfu));
    while (subdesc.size() <= s_id) {
        subdesc.emplace_back(static_cast<uint8_t>(4), GenSubIndex(getID(), static_cast<uint16_t>(subdesc.size())));
    }
    return get(s_id);
}

// ---------------------------------------------------------------------------------------------------------------------

GenDesc AccessUnit::Descriptor::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Descriptor::add(Subsequence &&sub) { subdesc.push_back(std::move(sub)); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Descriptor::set(uint16_t _id, Subsequence &&sub) {
    subdesc[_id] = std::move(sub);
}

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
    return descriptors[static_cast<uint8_t>(sub.first)].get(sub.second);
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Descriptor &AccessUnit::get(GenDesc desc) { return descriptors[static_cast<uint8_t>(desc)]; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Descriptor &AccessUnit::get(GenDesc desc) const { return descriptors[static_cast<uint8_t>(desc)]; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Subsequence &AccessUnit::get(GenSubIndex sub) const {
    return descriptors[static_cast<uint8_t>(sub.first)].get(sub.second);
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::set(GenSubIndex sub, Subsequence &&data) {
    descriptors[static_cast<uint8_t>(sub.first)].set(sub.second, std::move(data));
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::set(GenDesc sub, Descriptor &&data) { descriptors[static_cast<uint8_t>(sub)] = std::move(data); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::push(GenSubIndex sub, uint64_t value) { get(sub).push(value); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::pushDependency(GenSubIndex sub, uint64_t value) { get(sub).pushDependency(value); }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::isEnd(GenSubIndex sub) { return get(sub).end(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::peek(GenSubIndex sub, size_t lookahead) { return get(sub).get(lookahead); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::pull(GenSubIndex sub) { return get(sub).pull(); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::annotateNumSymbols(size_t num) { numSymbols = num; }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::Subsequence::isEmpty() const { return !getNumSymbols(); }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnit::Subsequence::getRawSize() const { return data.getRawSize(); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::write(util::BitWriter &writer) const {
    writer.writeAlignedBytes(data.getData(), data.getRawSize());
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(GenSubIndex _id, size_t size, util::BitReader &reader)
    : data(0, 1), id(std::move(_id)), numSymbols(0), dependency(0, 1) {
    data.resize(size);
    // no need to resize 'dependency' as it's not used on decoder side
    reader.readAlignedBytes(data.getData(), size);
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(GenSubIndex _id)
    : data(0, 1), id(std::move(_id)), numSymbols(0), dependency(0, 1) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(GenSubIndex _id, util::DataBlock &&dat)
    : data(std::move(dat)), id(std::move(_id)), numSymbols(0), dependency(0, data.getWordSize()) {}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::set(util::DataBlock &&dat) { data = std::move(dat); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::setPosition(size_t pos) { position = pos; }

// ---------------------------------------------------------------------------------------------------------------------

util::DataBlock &AccessUnit::Subsequence::getData() { return data; }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnit::Descriptor::getWrittenSize() const {
    size_t overhead = getDescriptor(getID()).tokentype ? 0 : (subdesc.size() - 1) * sizeof(uint32_t);
    return std::accumulate(subdesc.begin(), subdesc.end(), overhead, [](size_t sum, const Subsequence &payload) {
        return payload.isEmpty() ? sum : sum + payload.getRawSize();
    });
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Descriptor::write(util::BitWriter &writer) const {
    if (this->id == GenDesc::RNAME || this->id == GenDesc::MSAR) {
        subdesc.front().write(writer);
        return;
    }
    for (size_t i = 0; i < subdesc.size(); ++i) {
        if (i < (subdesc.size() - 1)) {
            writer.writeBits(subdesc[i].getRawSize(), 32);
        }
        subdesc[i].write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Descriptor::Descriptor(GenDesc _id, size_t count, size_t remainingSize, util::BitReader &reader) : id(_id) {
    if (this->id == GenDesc::RNAME || this->id == GenDesc::MSAR) {
        subdesc.emplace_back(GenSubIndex{_id, static_cast<uint16_t>(0)}, remainingSize, reader);
        return;
    }
    for (size_t i = 0; i < count; ++i) {
        size_t s = 0;
        if (i < (count - 1)) {
            s = reader.read<size_t>(32);
            remainingSize -= (s + 4);
        } else {
            s = remainingSize;
        }
        if (s) {
            subdesc.emplace_back(GenSubIndex{_id, static_cast<uint16_t>(i)}, s, reader);
        } else {
            subdesc.emplace_back(GenSubIndex{_id, static_cast<uint16_t>(i)}, util::DataBlock(0, 4));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::Descriptor::isEmpty() const {
    return std::all_of(subdesc.begin(), subdesc.end(), [](const auto &d) { return d.isEmpty(); });
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Descriptor::Descriptor() : id(static_cast<GenDesc>(0)) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(parameter::EncodingSet &&set, size_t _numRecords)
    : descriptors(), parameters(std::move(set)), numReads(_numRecords), minPos(0), maxPos(0), referenceSequence(0) {
    for (const auto &desc : getDescriptors()) {
        Descriptor desc_data(desc.id);
        for (const auto &subdesc : desc.subseqs) {
            auto bytes = range2bytes(core::getSubsequence(subdesc.id).range);
            desc_data.add(Subsequence(bytes, subdesc.id));
        }
        descriptors.push_back(std::move(desc_data));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setParameters(parameter::EncodingSet &&_parameters) { parameters = std::move(_parameters); }

// ---------------------------------------------------------------------------------------------------------------------

const parameter::EncodingSet &AccessUnit::getParameters() const { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

parameter::EncodingSet &AccessUnit::getParameters() { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

parameter::EncodingSet &&AccessUnit::moveParameters() { return std::move(parameters); }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnit::getNumReads() const { return numReads; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::clear() { *this = AccessUnit(parameter::EncodingSet(), 0); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::addRecord() { numReads++; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setReference(uint16_t ref) { referenceSequence = ref; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t AccessUnit::getReference() const { return referenceSequence; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setMaxPos(uint64_t pos) { maxPos = pos; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setMinPos(uint64_t pos) { minPos = pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::getMaxPos() const { return maxPos; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::getMinPos() const { return minPos; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setNumReads(size_t recs) { numReads = recs; }

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

stats::PerfStats &AccessUnit::getStats() { return stats; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setStats(stats::PerfStats &&_stats) { stats = std::move(_stats); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setReference(const ReferenceManager::ReferenceExcerpt &ex,
                              const std::vector<std::pair<size_t, size_t>> &ref2Write) {
    reference = ex;
    refToWrite = ref2Write;
}

// ---------------------------------------------------------------------------------------------------------------------

const ReferenceManager::ReferenceExcerpt &AccessUnit::getReferenceExcerpt() const { return reference; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::pair<size_t, size_t>> &AccessUnit::getRefToWrite() const { return refToWrite; }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::isReferenceOnly() const { return referenceOnly; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::setReferenceOnly(bool ref) { referenceOnly = ref; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
