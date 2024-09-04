/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/reference_metadata.h"
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

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

ReferenceMetadata::ReferenceMetadata(uint8_t _dataset_group_id, uint8_t _reference_id,
                                     std::string _reference_metadata_value)
    : dataset_group_id(_dataset_group_id),
      reference_id(_reference_id),
      reference_metadata_value(std::move(_reference_metadata_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadata::ReferenceMetadata(genie::util::BitReader& bitreader) {
    auto start_pos = bitreader.getStreamPosition() - 4;
    auto length = bitreader.readAlignedInt<uint64_t>();
    auto len_value = (length - sizeof(uint8_t) * 2);
    reference_metadata_value.resize(len_value);
    dataset_group_id = bitreader.readAlignedInt<uint8_t>();
    reference_id = bitreader.readAlignedInt<uint8_t>();
    bitreader.readAlignedBytes(reference_metadata_value.data(), reference_metadata_value.length());
    UTILS_DIE_IF(start_pos + length != uint64_t(bitreader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceMetadata::box_write(genie::util::BitWriter& bitWriter) const {
    bitWriter.writeAlignedInt<uint8_t>(dataset_group_id);
    bitWriter.writeAlignedInt<uint8_t>(reference_id);
    bitWriter.writeAlignedBytes(reference_metadata_value.data(), reference_metadata_value.length());
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ReferenceMetadata::getDatasetGroupID() const { return dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ReferenceMetadata::getReferenceID() const { return reference_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ReferenceMetadata::getReferenceMetadataValue() const { return reference_metadata_value; }

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceMetadata::patchID(uint8_t groupID) { dataset_group_id = groupID; }

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceMetadata::patchRefID(uint8_t _old, uint8_t _new) {
    if (reference_id == _old) {
        reference_id = _new;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::string ReferenceMetadata::decapsulate() { return std::move(reference_metadata_value); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
