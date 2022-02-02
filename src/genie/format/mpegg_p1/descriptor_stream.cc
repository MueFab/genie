/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/descriptor_stream.h"
#include <iostream>
#include <sstream>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

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
                                   const std::vector<MITClassConfig>& configs) {
    auto start_pos = reader.getPos() - 4;
    auto length = reader.read<uint64_t>();
    std::string tmp(4, '\0');
    reader.readBypass(tmp);
    UTILS_DIE_IF(tmp != "dshd", "Descriptor stream without header");
    header = DescriptorStreamHeader(reader);

    reader.readBypass(tmp);
    if (tmp == "dspr") {
        ds_protection = DSProtection(reader);
    } else {
        reader.setPos(reader.getPos() - 4);
    }

    uint8_t class_index = 0;
    uint8_t descriptor_index = 0;
    bool found = false;
    for (size_t i = 0; i < configs.size(); ++i) {
        if (configs[i].getClassID() == header.getClassType()) {
            class_index = i;
            found = true;
            bool found2 = false;
            for (size_t j = 0; j < configs[i].getDescriptorIDs().size(); ++j) {
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
                                                         length - (reader.getPos() - start_pos));
    UTILS_DIE_IF(payloadSizes.size() < header.getNumBlocks(), "DS payload sizes not available");
    for (size_t i = 0; i < header.getNumBlocks(); ++i) {
        payload.emplace_back(reader, payloadSizes[i]);
    }

    std::cout << start_pos << " " << length << " "  << reader.getPos() << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

const DescriptorStreamHeader& DescriptorStream::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStream::addPayload(Payload p) {
    payload.emplace_back(std::move(p));
    header.addBlock();
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Payload>& DescriptorStream::getPayloads() const { return payload; }

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStream::setProtection(DSProtection prot) { ds_protection = std::move(prot); }

// ---------------------------------------------------------------------------------------------------------------------

const DSProtection& DescriptorStream::getProtection() const { return *ds_protection; }

// ---------------------------------------------------------------------------------------------------------------------

bool DescriptorStream::hasProtection() const { return ds_protection != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStream::box_write(util::BitWriter& writer) const {
    header.write(writer);
    if (ds_protection != boost::none) {
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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
