/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "block-payload.h"

namespace genie {
// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::TransformedPayload::write(util::BitWriter& writer) const {
    for (size_t i = 0; i < payloadData.size(); ++i) {
        writer.write(payloadData.get(i), payloadData.getWordSize() * 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

BlockPayloadSet::TransformedPayload::TransformedPayload(util::DataBlock _data, size_t pos)
    : payloadData(std::move(_data)), position(pos) {}

// ---------------------------------------------------------------------------------------------------------------------

bool BlockPayloadSet::TransformedPayload::isEmpty() const { return payloadData.empty(); }

// ---------------------------------------------------------------------------------------------------------------------

util::DataBlock&& BlockPayloadSet::TransformedPayload::move() { return std::move(payloadData); }

// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::SubsequencePayload::write(util::BitWriter& writer) const {
    for (size_t i = 0; i < transformedPayloads.size(); ++i) {
        std::stringstream ss;
        util::BitWriter tmp_writer(&ss);
        transformedPayloads[i].write(writer);
        tmp_writer.flush();
        uint64_t bits = tmp_writer.getBitsWritten();
        if (i != transformedPayloads.size() - 1) {
            writer.write(bits / 8, 32);
        }
        writer.write(&ss);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::SubsequencePayload::add(TransformedPayload p) { transformedPayloads.emplace_back(std::move(p)); }

// ---------------------------------------------------------------------------------------------------------------------

bool BlockPayloadSet::SubsequencePayload::isEmpty() const {
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

std::vector<BlockPayloadSet::TransformedPayload>& BlockPayloadSet::SubsequencePayload::getTransformedPayloads() {
    return transformedPayloads;
}

// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::DescriptorPayload::write(util::BitWriter& writer) const {
    for (size_t i = 0; i < subsequencePayloads.size(); ++i) {
        std::stringstream ss;
        util::BitWriter tmp_writer(&ss);
        subsequencePayloads[i].write(tmp_writer);
        tmp_writer.flush();
        uint64_t bits = tmp_writer.getBitsWritten();
        if (i != subsequencePayloads.size() - 1) {
            writer.write(bits / 8, 32);
        }
        writer.write(&ss);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::DescriptorPayload::add(SubsequencePayload p) { subsequencePayloads.emplace_back(std::move(p)); }

// ---------------------------------------------------------------------------------------------------------------------

bool BlockPayloadSet::DescriptorPayload::isEmpty() const {
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

std::vector<BlockPayloadSet::SubsequencePayload>& BlockPayloadSet::DescriptorPayload::getSubsequencePayloads() {
    return subsequencePayloads;
}

// ---------------------------------------------------------------------------------------------------------------------

BlockPayloadSet::BlockPayloadSet(mpegg_p2::ParameterSet param, size_t _record_num)
    : desc_pay(genie::getDescriptors().size()), record_num(_record_num), parameters(std::move(param)) {}

// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::setPayload(genie::GenDesc i, DescriptorPayload p) { desc_pay[uint8_t(i)] = std::move(p); }

// ---------------------------------------------------------------------------------------------------------------------

BlockPayloadSet::DescriptorPayload& BlockPayloadSet::getPayload(genie::GenDesc i) { return desc_pay[uint8_t(i)]; }

// ---------------------------------------------------------------------------------------------------------------------

BlockPayloadSet::DescriptorPayload&& BlockPayloadSet::movePayload(size_t i) { return std::move(desc_pay[i]); }

// ---------------------------------------------------------------------------------------------------------------------

size_t BlockPayloadSet::getRecordNum() const { return record_num; }

// ---------------------------------------------------------------------------------------------------------------------

mpegg_p2::ParameterSet& BlockPayloadSet::getParameters() { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

const mpegg_p2::ParameterSet& BlockPayloadSet::getParameters() const { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

mpegg_p2::ParameterSet&& BlockPayloadSet::moveParameters() { return std::move(parameters); }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<BlockPayloadSet::DescriptorPayload>& BlockPayloadSet::getPayloads() { return desc_pay; }

}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------