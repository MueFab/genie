/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/descriptor_stream.h"
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

bool DescriptorStream::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DescriptorStream&>(info);
    return header == other.header && ds_protection == other.ds_protection && payload == other.payload;
}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStream::DescriptorStream(DescriptorStreamHeader _header) : header(std::move(_header)) {}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStream::DescriptorStream(util::BitReader& reader, const MasterIndexTable& table,
                                   const std::vector<dataset_header::MITClassConfig>& configs) {
    auto start_pos = reader.getStreamPosition() - 4;
    auto length = reader.read<uint64_t>();
    std::string tmp(4, '\0');
    reader.readAlignedBytes(tmp.data(), tmp.length());
    UTILS_DIE_IF(tmp != "dshd", "Descriptor stream without header");
    header = DescriptorStreamHeader(reader);

    reader.readAlignedBytes(tmp.data(), tmp.length());
    if (tmp == "dspr") {
        ds_protection = DescriptorStreamProtection(reader);
    } else {
        reader.setStreamPosition(reader.getStreamPosition() - 4);
    }

    uint8_t class_index = 0;
    uint8_t descriptor_index = 0;
    bool found = false;
    for (uint8_t i = 0; i < static_cast<uint8_t>(configs.size()); ++i) {
        if (configs[i].getClassID() == header.getClassType()) {
            class_index = i;
            found = true;
            bool found2 = false;
            for (uint8_t j = 0; j < static_cast<uint8_t>(configs[i].getDescriptorIDs().size()); ++j) {
                if (configs[i].getDescriptorIDs()[j] == header.getDescriptorID()) {
                    descriptor_index = j;
                    found2 = true;
                    break;
                }
            }
            UTILS_DIE_IF(!found2, "Did not find descriptor config");
            break;
        }
    }
    UTILS_DIE_IF(!found, "Did not find class config");

    auto payloadSizes = table.getDescriptorStreamOffsets(class_index, descriptor_index,
                                                         header.getClassType() == core::record::ClassType::CLASS_U,
                                                         length - (reader.getStreamPosition() - start_pos));
    UTILS_DIE_IF(payloadSizes.size() < header.getNumBlocks(), "DS payload sizes not available");
    for (size_t i = 0; i < header.getNumBlocks(); ++i) {
        payload.emplace_back(reader, payloadSizes[i]);
    }

    UTILS_DIE_IF(start_pos + length != static_cast<uint64_t>(reader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

const DescriptorStreamHeader& DescriptorStream::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStream::addPayload(core::Payload p) {
    payload.emplace_back(std::move(p));
    header.addBlock();
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<core::Payload>& DescriptorStream::getPayloads() const { return payload; }

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStream::setProtection(DescriptorStreamProtection prot) { ds_protection = std::move(prot); }

// ---------------------------------------------------------------------------------------------------------------------

const DescriptorStreamProtection& DescriptorStream::getProtection() const { return *ds_protection; }

// ---------------------------------------------------------------------------------------------------------------------

bool DescriptorStream::hasProtection() const { return ds_protection != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStream::box_write(util::BitWriter& writer) const {
    header.write(writer);
    if (ds_protection != std::nullopt) {
        ds_protection->write(writer);
    }
    for (const auto& p : payload) {
        p.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DescriptorStream::getKey() const {
    static const std::string key = "dscn";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStream::DescriptorStream(genie::core::GenDesc descriptor, genie::core::record::ClassType classID,
                                   std::vector<format::mgb::Block> blocks)
    : header(false, descriptor, classID, static_cast<uint32_t>(blocks.size())) {
    for (auto& b : blocks) {
        payload.emplace_back(b.movePayloadUnparsed());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<format::mgb::Block> DescriptorStream::decapsulate() {
    std::vector<format::mgb::Block> ret;
    ret.reserve(payload.size());
    for (auto& p : payload) {
        ret.emplace_back(header.getDescriptorID(), std::move(p));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

bool DescriptorStream::isEmpty() const {
    if (payload.empty()) {
        return true;
    }

    return std::all_of(payload.begin(), payload.end(), [](const auto& p) { return p.getPayloadSize() == 0; });
}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStreamProtection& DescriptorStream::getProtection() { return *ds_protection; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
