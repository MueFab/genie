/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_header/mit_class_config.h"
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

// ---------------------------------------------------------------------------------------------------------------------

bool MITClassConfig::operator==(const MITClassConfig& other) const {
    return id == other.id && descriptor_ids == other.descriptor_ids;
}

// ---------------------------------------------------------------------------------------------------------------------

MITClassConfig::MITClassConfig(const core::record::ClassType _id) : id(_id) {}

// ---------------------------------------------------------------------------------------------------------------------

MITClassConfig::MITClassConfig(util::BitReader& reader, const bool block_header_flag) {
    id = reader.read<core::record::ClassType>(4);
    if (!block_header_flag) {
        const auto num_descriptors = reader.read<uint8_t>(5);
        for (size_t i = 0; i < num_descriptors; ++i) {
            descriptor_ids.emplace_back(reader.read<core::GenDesc>(7));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MITClassConfig::write(util::BitWriter& writer) const {
    writer.writeBits(static_cast<uint8_t>(id), 4);
    if (!descriptor_ids.empty()) {
        writer.writeBits(descriptor_ids.size(), 5);
        for (const auto& d : descriptor_ids) {
            writer.writeBits(static_cast<uint8_t>(d), 7);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MITClassConfig::addDescriptorID(core::GenDesc desc) { descriptor_ids.emplace_back(desc); }

// ---------------------------------------------------------------------------------------------------------------------

core::record::ClassType MITClassConfig::getClassID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<core::GenDesc>& MITClassConfig::getDescriptorIDs() const { return descriptor_ids; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
