/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/reference/location/external/mpeg.h"
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference::location::external {

// ---------------------------------------------------------------------------------------------------------------------

MPEG::MPEG(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, uint8_t _group_id, uint16_t _dataset_id,
           std::string _ref_checksum, genie::core::MPEGMinorVersion _version)
    : External(_reserved, std::move(_uri), algo, RefType::MPEGG_REF),
      version(_version),
      external_dataset_group_id(_group_id),
      external_dataset_id(_dataset_id),
      ref_checksum(std::move(_ref_checksum)) {
    UTILS_DIE_IF(version == genie::core::MPEGMinorVersion::V1900 &&
                     ref_checksum.size() != checksum_sizes[static_cast<uint8_t>(algo)],
                 "Invalid reference checksum");
}

// ---------------------------------------------------------------------------------------------------------------------

MPEG::MPEG(genie::util::BitReader& reader, size_t seq_count, genie::core::MPEGMinorVersion _version)
    : External(reader),
      version(_version),
      ref_checksum(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0') {
    external_dataset_group_id = reader.readAlignedInt<uint8_t>();
    external_dataset_id = reader.readAlignedInt<uint16_t>();
    if (version == core::MPEGMinorVersion::V1900) {
        reader.readAlignedBytes(ref_checksum.data(), ref_checksum.size());
    } else {
        ref_checksum.clear();
        for (size_t i = 0; i < seq_count; ++i) {
            seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
            reader.readAlignedBytes(seq_checksums.back().data(), seq_checksums.back().size());
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

MPEG::MPEG(genie::util::BitReader& reader, uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo,
           size_t seq_count, genie::core::MPEGMinorVersion _version)
    : External(_reserved, std::move(_uri), algo, RefType::MPEGG_REF),
      version(_version),
      ref_checksum(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0') {
    external_dataset_group_id = reader.readAlignedInt<uint8_t>();
    external_dataset_id = reader.readAlignedInt<uint16_t>();
    if (version == core::MPEGMinorVersion::V1900) {
        reader.readAlignedBytes(ref_checksum.data(), ref_checksum.length());
    } else {
        ref_checksum.clear();
        for (size_t i = 0; i < seq_count; ++i) {
            seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
            reader.readAlignedBytes(seq_checksums.back().data(), seq_checksums.back().length());
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t MPEG::getExternalDatasetGroupID() const { return external_dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t MPEG::getExternalDatasetID() const { return external_dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& MPEG::getRefChecksum() const { return ref_checksum; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& MPEG::getSeqChecksums() const { return seq_checksums; }

// ---------------------------------------------------------------------------------------------------------------------

void MPEG::addSeqChecksum(std::string checksum) {
    UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                 "Invalid checksum length");
    seq_checksums.emplace_back(std::move(checksum));
}

// ---------------------------------------------------------------------------------------------------------------------

void MPEG::write(genie::util::BitWriter& writer) {
    External::write(writer);
    writer.writeAlignedInt(external_dataset_group_id);
    writer.writeAlignedInt(external_dataset_id);
    if (version == core::MPEGMinorVersion::V1900) {
        writer.writeAlignedBytes(ref_checksum.data(), ref_checksum.length());
    } else {
        for (const auto& s : seq_checksums) {
            writer.writeAlignedBytes(s.data(), s.length());
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MPEG::addChecksum(std::string checksum) {
    UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                 "Invalid checksum length");
    if (version != core::MPEGMinorVersion::V1900) {
        seq_checksums.emplace_back(std::move(checksum));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::meta::RefBase> MPEG::decapsulate() {
    auto ret = std::make_unique<genie::core::meta::external_ref::MPEG>(
        std::move(getURI()), genie::core::meta::ExternalRef::ChecksumAlgorithm(getChecksumAlgorithm()),
        external_dataset_group_id, external_dataset_id, std::move(ref_checksum));
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location::external

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
