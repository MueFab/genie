/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference/location/external/raw.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference::location::external {

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::meta::RefBase> Raw::decapsulate() {
    auto ret = std::make_unique<core::meta::external_ref::Raw>(
        std::move(getURI()), static_cast<core::meta::ExternalRef::ChecksumAlgorithm>(getChecksumAlgorithm()));

    for (auto& s : seq_checksums) {
        ret->addChecksum(std::move(s));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

Raw::Raw(const uint8_t _reserved, std::string _uri, const ChecksumAlgorithm algo)
    : External(_reserved, std::move(_uri), algo, RefType::RAW_REF) {}

// ---------------------------------------------------------------------------------------------------------------------

Raw::Raw(util::BitReader& reader, const size_t seq_count) : External(reader) {
    for (size_t i = 0; i < seq_count; ++i) {
        seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
        reader.readAlignedBytes(seq_checksums.back().data(), seq_checksums.back().size());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Raw::Raw(util::BitReader& reader, const uint8_t _reserved, std::string _uri, const ChecksumAlgorithm algo,
         const size_t seq_count)
    : External(_reserved, std::move(_uri), algo, RefType::RAW_REF) {
    for (size_t i = 0; i < seq_count; ++i) {
        seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
        reader.readAlignedBytes(seq_checksums.back().data(), seq_checksums.back().length());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& Raw::getSeqChecksums() const { return seq_checksums; }

// ---------------------------------------------------------------------------------------------------------------------

void Raw::addSeqChecksum(std::string checksum) {
    UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                 "Invalid checksum length");
    seq_checksums.emplace_back(std::move(checksum));
}

// ---------------------------------------------------------------------------------------------------------------------

void Raw::write(util::BitWriter& writer) {
    External::write(writer);
    for (const auto& s : seq_checksums) {
        writer.writeAlignedBytes(s.data(), s.length());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Raw::addChecksum(std::string checksum) {
    UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                 "Invalid checksum length");
    seq_checksums.emplace_back(std::move(checksum));
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location::external

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
