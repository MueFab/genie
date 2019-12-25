/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "block-payload.h"

// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::TransformedPayload::write(util::BitWriter* writer) const {
    for (size_t i = 0; i < payloadData.size(); ++i) {
        writer->write(payloadData.get(i), payloadData.getWordSize() * 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

BlockPayloadSet::TransformedPayload::TransformedPayload(gabac::DataBlock* _data) : payloadData(0, 1) {
    payloadData.swap(_data);
}

// ---------------------------------------------------------------------------------------------------------------------

bool BlockPayloadSet::TransformedPayload::isEmpty() const { return payloadData.empty(); }

// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::TransformedPayload::swap(gabac::DataBlock* _data) { payloadData.swap(_data); }

// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::SubsequencePayload::write(util::BitWriter* writer) const {
    for (size_t i = 0; i < transformedPayloads.size(); ++i) {
        std::stringstream ss;
        util::BitWriter tmp_writer(&ss);
        transformedPayloads[i]->write(writer);
        tmp_writer.flush();
        uint64_t bits = tmp_writer.getBitsWritten();
        if (i != transformedPayloads.size() - 1) {
            writer->write(bits / 8, 32);
        }
        writer->write(&ss);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::SubsequencePayload::add(std::unique_ptr<TransformedPayload> p) {
    transformedPayloads.push_back(std::move(p));
}

// ---------------------------------------------------------------------------------------------------------------------

bool BlockPayloadSet::SubsequencePayload::isEmpty() const {
    if (transformedPayloads.empty()) {
        return true;
    }
    for (const auto& p : transformedPayloads) {
        if (!p->isEmpty()) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::unique_ptr<BlockPayloadSet::TransformedPayload>>&
BlockPayloadSet::SubsequencePayload::getTransformedPayloads() const {
    return transformedPayloads;
}

// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::DescriptorPayload::write(util::BitWriter* writer) const {
    for (size_t i = 0; i < subsequencePayloads.size(); ++i) {
        std::stringstream ss;
        util::BitWriter tmp_writer(&ss);
        subsequencePayloads[i]->write(writer);
        tmp_writer.flush();
        uint64_t bits = tmp_writer.getBitsWritten();
        if (i != subsequencePayloads.size() - 1) {
            writer->write(bits / 8, 32);
        }
        writer->write(&ss);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::DescriptorPayload::add(std::unique_ptr<SubsequencePayload> p) {
    subsequencePayloads.push_back(std::move(p));
}

// ---------------------------------------------------------------------------------------------------------------------

bool BlockPayloadSet::DescriptorPayload::isEmpty() const {
    if (subsequencePayloads.empty()) {
        return true;
    }
    for (const auto& p : subsequencePayloads) {
        if (!p->isEmpty()) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::unique_ptr<BlockPayloadSet::SubsequencePayload>>&
BlockPayloadSet::DescriptorPayload::getSubsequencePayloads() const {
    return subsequencePayloads;
}

// ---------------------------------------------------------------------------------------------------------------------

BlockPayloadSet::BlockPayloadSet(std::unique_ptr<format::mpegg_p2::ParameterSet> param, size_t _record_num)
    : desc_pay(getDescriptors().size()), record_num(_record_num), parameters(std::move(param)) {}

// ---------------------------------------------------------------------------------------------------------------------

void BlockPayloadSet::setPayload(GenomicDescriptor i, std::unique_ptr<DescriptorPayload> p) {
    desc_pay[uint8_t(i)] = std::move(p);
}

// ---------------------------------------------------------------------------------------------------------------------

BlockPayloadSet::DescriptorPayload* BlockPayloadSet::getPayload(GenomicDescriptor i) {
    return desc_pay[uint8_t(i)].get();
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<BlockPayloadSet::DescriptorPayload> BlockPayloadSet::movePayload(size_t i) {
    return std::move(desc_pay[i]);
}

// ---------------------------------------------------------------------------------------------------------------------

size_t BlockPayloadSet::getRecordNum() const { return record_num; }

// ---------------------------------------------------------------------------------------------------------------------

format::mpegg_p2::ParameterSet* BlockPayloadSet::getParameters() { return parameters.get(); }

// ---------------------------------------------------------------------------------------------------------------------

const format::mpegg_p2::ParameterSet* BlockPayloadSet::getParameters() const { return parameters.get(); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<format::mpegg_p2::ParameterSet> BlockPayloadSet::moveParameters() { return std::move(parameters); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::unique_ptr<BlockPayloadSet::DescriptorPayload>>& BlockPayloadSet::getPayloads() const {
    return desc_pay;
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------