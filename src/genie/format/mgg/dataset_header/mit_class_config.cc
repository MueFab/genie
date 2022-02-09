/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/dataset_header/mit_class_config.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace dataset_header {

// ---------------------------------------------------------------------------------------------------------------------

bool MITClassConfig::operator==(const MITClassConfig& other) const {
    return id == other.id && descriptor_ids == other.descriptor_ids;
}

// ---------------------------------------------------------------------------------------------------------------------

MITClassConfig::MITClassConfig(genie::core::record::ClassType _id) : id(_id) {}

// ---------------------------------------------------------------------------------------------------------------------

MITClassConfig::MITClassConfig(genie::util::BitReader& reader, bool block_header_flag) {
    id = reader.read<genie::core::record::ClassType>(4);
    if (!block_header_flag) {
        auto num_descriptors = reader.read<uint8_t>(5);
        for (size_t i = 0; i < num_descriptors; ++i) {
            descriptor_ids.emplace_back(reader.read<genie::core::GenDesc>(7));
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MITClassConfig::write(genie::util::BitWriter& writer) const {
    writer.write(static_cast<uint8_t>(id), 4);
    if (!descriptor_ids.empty()) {
        writer.write(descriptor_ids.size(), 5);
        for (const auto& d : descriptor_ids) {
            writer.write(static_cast<uint8_t>(d), 7);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MITClassConfig::addDescriptorID(genie::core::GenDesc desc) { descriptor_ids.emplace_back(desc); }

// ---------------------------------------------------------------------------------------------------------------------

genie::core::record::ClassType MITClassConfig::getClassID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<genie::core::GenDesc>& MITClassConfig::getDescriptorIDs() const { return descriptor_ids; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace dataset_header
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
