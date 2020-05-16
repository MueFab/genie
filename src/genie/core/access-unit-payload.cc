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

AccessUnitPayload::SubsequencePayload::SubsequencePayload(GenSubIndex _id) : id(std::move(_id)), numSymbols(0) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitPayload::SubsequencePayload::SubsequencePayload(GenSubIndex _id, util::DataBlock&& _payload)
    : id(std::move(_id)), payload(std::move(_payload)), numSymbols(0) {}

// ---------------------------------------------------------------------------------------------------------------------

AccessUnitPayload::SubsequencePayload::SubsequencePayload(GenSubIndex _id, size_t size, util::BitReader& reader)
    : id(std::move(_id)), payload(0, 1), numSymbols(0) {
    payload.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        payload.push_back(reader.read(8));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

GenSubIndex AccessUnitPayload::SubsequencePayload::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::SubsequencePayload::write(util::BitWriter& writer) const {
    writer.writeBuffer(payload.getData(), payload.getRawSize());
}

// ---------------------------------------------------------------------------------------------------------------------

bool AccessUnitPayload::SubsequencePayload::isEmpty() const {
    return payload.empty();
}

// ---------------------------------------------------------------------------------------------------------------------

void AccessUnitPayload::DescriptorPayload::write(util::BitWriter& writer) const {
    if(this->id == GenDesc::RNAME || this->id == GenDesc::MSAR) {
        subsequencePayloads.front().write(writer);
        return;
    }
    for (size_t i = 0; i < subsequencePayloads.size(); ++i) {
        if (i < (subsequencePayloads.size() - 1)) {
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
    if(this->id == GenDesc::RNAME || this->id == GenDesc::MSAR) {
        subsequencePayloads.emplace_back(GenSubIndex{_id, 0}, remainingSize, reader);
     /*   size_t numSymbols = reader.read<uint32_t>();
        auto num_streams = reader.read<uint16_t>();
        int32_t typenum = -1;
        for (size_t i = 0; i < num_streams; ++i) {
            uint8_t type = reader.read(4);

            if(!type) {
                typenum ++;
            }

            while(subsequencePayloads.size() < ((uint32_t(typenum) << 4u) | type)) {
                subsequencePayloads.emplace_back(GenSubIndex {id, subsequencePayloads.size()});
                subsequencePayloads.back().annotateNumSymbols(numSymbols);
            }

            uint8_t method = reader.read(4);

            UTILS_DIE_IF(method != 3, "Only CABAC method supported");

            subsequencePayloads.emplace_back(GenSubIndex{_id, subsequencePayloads.size()}, 0, reader);
            subsequencePayloads.back().annotateNumSymbols(numSymbols);
        }
        return; */
    }
    for (size_t i = 0; i < count; ++i) {
        size_t s = 0;
        if (i < (count - 1)) {
            s = reader.read(32);
            remainingSize -= (s + 4);
        } else {
            s = remainingSize;
        }
        if (s) {
            subsequencePayloads.emplace_back(GenSubIndex{_id, i}, s, reader);
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
