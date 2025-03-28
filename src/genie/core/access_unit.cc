/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <utility>
#include "genie/core/access_unit.h"
#include "genie/core/mismatch_decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence &AccessUnit::Subsequence::operator=(const Subsequence &sub) {
    data_ = sub.data_;
    position_ = sub.position_;
    id_ = sub.id_;
    numSymbols = sub.numSymbols;
    dependency = sub.dependency;
    if (sub.mmDecoder != nullptr) mmDecoder = sub.mmDecoder->Copy();
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence &AccessUnit::Subsequence::operator=(AccessUnit::Subsequence &&sub) noexcept {
    data_ = std::move(sub.data_);
    position_ = sub.position_;
    id_ = sub.id_;
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

util::DataBlock *AccessUnit::Subsequence::GetDependency() {
    if (id_ == gen_sub::kMismatchTypeSubstBase || id_ == gen_sub::kRefTransTransform)
        return &dependency;
    else
        return nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit::Subsequence AccessUnit::Subsequence::AttachMismatchDecoder(std::unique_ptr<MismatchDecoder> mm) {
    mmDecoder = std::move(mm);
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

MismatchDecoder *AccessUnit::Subsequence::getMismatchDecoder() const { return mmDecoder.get(); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(uint8_t wordSize, GenSubIndex _id)
    : data_(0, wordSize), position_(0), id_(std::move(_id)), dependency(0, wordSize) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(util::DataBlock d, GenSubIndex _id)
    : data_(std::move(d)), position_(0), id_(std::move(_id)), dependency(0, (uint8_t)d.GetWordSize()) {}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::push(uint64_t val) { data_.PushBack(val); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::pushDependency(uint64_t val) { dependency.PushBack(val); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::inc() { position_++; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::Subsequence::get(size_t lookahead) const { return data_.Get(position_ + lookahead); }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::Subsequence::end() const { return data_.Size() <= position_; }

// ---------------------------------------------------------------------------------------------------------------------

util::DataBlock &&AccessUnit::Subsequence::Move() { return std::move(data_); }

// ---------------------------------------------------------------------------------------------------------------------

GenSubIndex AccessUnit::Subsequence::GetId() const { return id_; }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnit::Subsequence::GetNumSymbols() const { return data_.Size(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnit::Subsequence::pull() {
    if (end()) {
        UTILS_DIE("Tried to read descriptor that has already ended");
    }
    return data_.Get(position_++);
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence &AccessUnit::Descriptor::Get(uint16_t sub) { return subdesc[sub]; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Subsequence &AccessUnit::Descriptor::Get(uint16_t sub) const { return subdesc[sub]; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence &AccessUnit::Descriptor::getTokenType(uint16_t pos, uint8_t _type) {
    uint16_t s_id = ((pos << 4u) | (_type & 0xfu));
    while (subdesc.size() <= s_id) {
        subdesc.emplace_back((uint8_t)4, GenSubIndex(GetId(), uint16_t(subdesc.size())));
    }
    return Get(s_id);
}

// ---------------------------------------------------------------------------------------------------------------------

GenDesc AccessUnit::Descriptor::GetId() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Descriptor::Add(Subsequence &&sub) { subdesc.push_back(std::move(sub)); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Descriptor::Set(uint16_t _id, Subsequence &&sub) { subdesc[uint16_t(_id)] = std::move(sub); }

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

size_t AccessUnit::Descriptor::GetSize() const { return subdesc.size(); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence &AccessUnit::get(GenSubIndex sub) { return descriptors[uint8_t(sub.first)].Get(sub.second); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Descriptor &AccessUnit::get(GenDesc desc) { return descriptors[uint8_t(desc)]; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Descriptor &AccessUnit::get(GenDesc desc) const { return descriptors[uint8_t(desc)]; }

// ---------------------------------------------------------------------------------------------------------------------

const AccessUnit::Subsequence &AccessUnit::get(GenSubIndex sub) const {
    return descriptors[uint8_t(sub.first)].Get(sub.second);
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Set(GenSubIndex sub, Subsequence &&data) {
    descriptors[uint8_t(sub.first)].Set(sub.second, std::move(data));
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Set(GenDesc sub, Descriptor &&data) { descriptors[uint8_t(sub)] = std::move(data); }

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

void AccessUnit::Subsequence::AnnotateNumSymbols(size_t num) { numSymbols = num; }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::Subsequence::IsEmpty() const { return !GetNumSymbols(); }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnit::Subsequence::GetRawSize() const { return data_.GetRawSize(); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::Write(util::BitWriter &writer) const {
    writer.WriteAlignedBytes(data_.GetData(), data_.GetRawSize());
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(GenSubIndex _id, size_t size, util::BitReader &reader)
    : data_(0, 1), id_(std::move(_id)), numSymbols(0), dependency(0, 1) {
    data_.Resize(size);
    // no need to resize 'dependency' as it's not used on decoder side
    reader.ReadAlignedBytes(reinterpret_cast<char *>(data_.GetData()), size);
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(GenSubIndex _id) : data_(0, 1), id_(_id), numSymbols(0), dependency(0, 1) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(GenSubIndex _id, util::DataBlock &&dat)
    : data_(std::move(dat)), id_(std::move(_id)), numSymbols(0), dependency(0, (uint8_t)data_.GetWordSize()) {}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::Set(util::DataBlock &&dat) { data_ = std::move(dat); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Subsequence::setPosition(size_t pos) { position_ = pos; }

// ---------------------------------------------------------------------------------------------------------------------

util::DataBlock &AccessUnit::Subsequence::getData() { return data_; }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnit::Descriptor::getWrittenSize() const {
    size_t overhead = GetDescriptor(GetId()).token_type ? 0 : (subdesc.size() - 1) * sizeof(uint32_t);
    return std::accumulate(subdesc.begin(), subdesc.end(), overhead, [](size_t sum, const Subsequence &payload) {
        return payload.IsEmpty() ? sum : sum + payload.GetRawSize();
    });
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnit::Descriptor::write(util::BitWriter &writer) const {
    if (this->id == GenDesc::kReadName || this->id == GenDesc::kMultiSegmentAlignment) {
        subdesc.front().Write(writer);
        return;
    }
    for (size_t i = 0; i < subdesc.size(); ++i) {
        if (i < (subdesc.size() - 1)) {
            writer.WriteBits(subdesc[i].GetRawSize(), 32);
        }
        subdesc[i].Write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Descriptor::Descriptor(GenDesc _id, size_t count, size_t remainingSize, util::BitReader &reader) : id(_id) {
    if (this->id == GenDesc::kReadName || this->id == GenDesc::kMultiSegmentAlignment) {
        subdesc.emplace_back(GenSubIndex{_id, (uint16_t)0}, remainingSize, reader);
        return;
    }
    for (size_t i = 0; i < count; ++i) {
        size_t s = 0;
        if (i < (count - 1)) {
            s = reader.Read<size_t>(32);
            remainingSize -= (s + 4);
        } else {
            s = remainingSize;
        }
        if (s) {
            subdesc.emplace_back(GenSubIndex{_id, (uint16_t)i}, s, reader);
        } else {
            subdesc.emplace_back(GenSubIndex{_id, (uint16_t)i}, util::DataBlock(0, 4));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnit::Descriptor::isEmpty() const {
    for (const auto &d : subdesc) {
        if (!d.IsEmpty()) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::Descriptor::Descriptor() : id(GenDesc(0)) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnit::AccessUnit(parameter::EncodingSet &&set, size_t _numRecords)
    : descriptors(), parameters(std::move(set)), numReads(_numRecords), minPos(0), maxPos(0), referenceSequence(0) {
    for (const auto &desc : GetDescriptors()) {
        Descriptor desc_data(desc.id);
        for (const auto &subdesc : desc.sub_seqs) {
            auto bytes = Range2Bytes(core::GetSubsequence(subdesc.id).range);
            desc_data.Add(Subsequence(bytes, subdesc.id));
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

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
