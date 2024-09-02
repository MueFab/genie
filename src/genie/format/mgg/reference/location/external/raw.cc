/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/reference/location/external/raw.h"
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference::location::external {

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::meta::RefBase> Raw::decapsulate() {
    auto ret = std::make_unique<genie::core::meta::external_ref::Raw>(
        std::move(getURI()), genie::core::meta::ExternalRef::ChecksumAlgorithm(getChecksumAlgorithm()));

    for (auto& s : seq_checksums) {
        ret->addChecksum(std::move(s));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

Raw::Raw(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo)
    : External(_reserved, std::move(_uri), algo, RefType::RAW_REF) {}

// ---------------------------------------------------------------------------------------------------------------------

Raw::Raw(genie::util::BitReader& reader, size_t seq_count) : location::External(reader) {
    for (size_t i = 0; i < seq_count; ++i) {
        seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
        reader.readBypass(seq_checksums.back());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Raw::Raw(genie::util::BitReader& reader, uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, size_t seq_count)
    : External(_reserved, std::move(_uri), algo, RefType::RAW_REF) {
    for (size_t i = 0; i < seq_count; ++i) {
        seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
        reader.readBypass(seq_checksums.back());
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

void Raw::write(genie::util::BitWriter& writer) {
    External::write(writer);
    for (const auto& s : seq_checksums) {
        writer.writeBypass(s.data(), s.length());
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
