/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "access-unit-payload.h"
#include <sstream>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::TransformedPayload::write(util::BitWriter& writer) const {
    writer.writeBypass(payloadData.getData(), payloadData.getRawSize());
 /*   for (size_t i = 0; i < payloadData.size(); ++i) {
        writer.write(payloadData.get(i), static_cast<uint8_t>(payloadData.getWordSize() * 8));
    }*/
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitPayload::TransformedPayload::TransformedPayload(util::DataBlock _data, size_t pos)
    : payloadData(std::move(_data)), position(pos) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitPayload::TransformedPayload::TransformedPayload(size_t size, util::BitReader& reader) {
    payloadData = util::DataBlock(0, 1);
    payloadData.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        payloadData.push_back(reader.read(8));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnitPayload::TransformedPayload::isEmpty() const { return payloadData.empty(); }

// ---------------------------------------------------------------------------------------------------------------------

util::DataBlock&& AccessUnitPayload::TransformedPayload::move() { return std::move(payloadData); }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnitPayload::TransformedPayload::getIndex() const { return position; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitPayload::SubsequencePayload::SubsequencePayload(GenSubIndex _id) : id(std::move(_id)) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitPayload::SubsequencePayload::SubsequencePayload(GenSubIndex _id, size_t remainingSize, size_t subseq_ctr,
                                                          util::BitReader& reader)
    : id(std::move(_id)) {
    for (size_t i = 0; i < subseq_ctr; ++i) {
        size_t s = 0;
        if (i != subseq_ctr - 1) {
            s = reader.read(32);
            remainingSize -= (s + 4);
        } else {
            s = remainingSize;
        }
        if (s) {
            transformedPayloads.emplace_back(s, reader);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

GenSubIndex AccessUnitPayload::SubsequencePayload::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::SubsequencePayload::write(util::BitWriter& writer) const {
    for (size_t i = 0; i < transformedPayloads.size(); ++i) {
        transformedPayloads[i].write(writer);
        if (i != transformedPayloads.size() - 1) {
            writer.write(transformedPayloads[i].getWrittenSize(), 32);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::SubsequencePayload::add(TransformedPayload p) {
    transformedPayloads.emplace_back(std::move(p));
}

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnitPayload::SubsequencePayload::isEmpty() const {
    if (transformedPayloads.empty()) {
        return true;
    }
    for (const auto& p : transformedPayloads) {
        if (!p.isEmpty()) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::DescriptorPayload::write(util::BitWriter& writer) const {
    for (size_t i = 0; i < subsequencePayloads.size(); ++i) {
        if (i != subsequencePayloads.size() - 1) {
            writer.write(subsequencePayloads[i].getWrittenSize(), 32);
        }
        subsequencePayloads[i].write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::DescriptorPayload::add(SubsequencePayload p) { subsequencePayloads.emplace_back(std::move(p)); }

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnitPayload::DescriptorPayload::isEmpty() const {
    if (subsequencePayloads.empty()) {
        return true;
    }
    for (const auto& p : subsequencePayloads) {
        if (!p.isEmpty()) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitPayload::AccessUnitPayload(parameter::ParameterSet param, size_t _record_num)
    : desc_pay(getDescriptors().size()), record_num(_record_num), parameters(std::move(param)) {}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::setPayload(GenDesc i, DescriptorPayload p) { desc_pay[uint8_t(i)] = std::move(p); }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitPayload::DescriptorPayload& AccessUnitPayload::getPayload(GenDesc i) { return desc_pay[uint8_t(i)]; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitPayload::DescriptorPayload&& AccessUnitPayload::movePayload(size_t i) { return std::move(desc_pay[i]); }

// ---------------------------------------------------------------------------------------------------------------------

size_t AccessUnitPayload::getRecordNum() const { return record_num; }

// ---------------------------------------------------------------------------------------------------------------------

parameter::ParameterSet& AccessUnitPayload::getParameters() { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

const parameter::ParameterSet& AccessUnitPayload::getParameters() const { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

parameter::ParameterSet&& AccessUnitPayload::moveParameters() { return std::move(parameters); }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::clear() { *this = AccessUnitPayload(parameter::ParameterSet(), 0); }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t AccessUnitPayload::getReference() const { return reference; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::setReference(uint16_t ref) { reference = ref; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::setMaxPos(uint64_t pos) { maxPos = pos; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::setMinPos(uint64_t pos) { minPos = pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnitPayload::getMaxPos() const { return maxPos; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AccessUnitPayload::getMinPos() const { return minPos; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::setRecordNum(size_t num) { record_num = num; }

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitPayload::DescriptorPayload::DescriptorPayload(GenDesc _id) : id(_id) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitPayload::DescriptorPayload::DescriptorPayload() : id(GenDesc(0)) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitPayload::DescriptorPayload::DescriptorPayload(GenDesc _id, size_t count, size_t remainingSize,
                                                        util::BitReader& reader)
    : id(_id) {
    for (size_t i = 0; i < count; ++i) {
        size_t s = 0;
        if (i != count - 1) {
            s = reader.read(32);
            remainingSize -= (s + 4);
        } else {
            s = remainingSize;
        }
        if (s) {
            subsequencePayloads.emplace_back(GenSubIndex{_id, i}, s, 1, reader);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

GenDesc AccessUnitPayload::DescriptorPayload::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------