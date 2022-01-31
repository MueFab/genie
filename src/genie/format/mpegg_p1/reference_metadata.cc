/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference_metadata.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

bool ReferenceMetadata::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const ReferenceMetadata&>(info);
    return dataset_group_id == other.dataset_group_id && reference_id == other.reference_id &&
           reference_metadata_value == other.reference_metadata_value;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ReferenceMetadata::getKey() const {
    static const std::string key = "rfmd";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ReferenceMetadata::getSize() const {
    return GenInfo::getSize() + sizeof(uint8_t) * 2 + reference_metadata_value.length();
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadata::ReferenceMetadata(uint8_t _dataset_group_id, uint8_t _reference_id,
                                     std::string _reference_metadata_value)
    : dataset_group_id(_dataset_group_id),
      reference_id(_reference_id),
      reference_metadata_value(std::move(_reference_metadata_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadata::ReferenceMetadata(genie::util::BitReader& bitreader) {
    auto length = bitreader.readBypassBE<uint64_t>();
    auto len_value = (length - sizeof(uint8_t) * 2);
    reference_metadata_value.resize(len_value);
    dataset_group_id = bitreader.readBypassBE<uint8_t>();
    reference_id = bitreader.readBypassBE<uint8_t>();
    bitreader.readBypass(reference_metadata_value);
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceMetadata::write(genie::util::BitWriter& bitWriter) const {
    GenInfo::write(bitWriter);
    bitWriter.writeBypassBE<uint8_t>(dataset_group_id);
    bitWriter.writeBypassBE<uint8_t>(reference_id);
    bitWriter.writeBypass(reference_metadata_value.data(), reference_metadata_value.length());
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ReferenceMetadata::getDatasetGroupID() const { return dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ReferenceMetadata::getReferenceID() const { return reference_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ReferenceMetadata::getReferenceMetadataValue() const { return reference_metadata_value; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------