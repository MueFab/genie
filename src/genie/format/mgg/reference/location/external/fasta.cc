/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <utility>
#include "genie/format/mgg/reference/location/external/fasta.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace reference {
namespace location {
namespace external {

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<genie::core::meta::RefBase> Fasta::decapsulate() {
    auto ret = genie::util::make_unique<genie::core::meta::external_ref::Fasta>(
        std::move(getURI()), genie::core::meta::ExternalRef::ChecksumAlgorithm(getChecksumAlgorithm()));

    for (auto& s : seq_checksums) {
        ret->addChecksum(std::move(s));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

Fasta::Fasta(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo)
    : External(_reserved, std::move(_uri), algo, RefType::FASTA_REF) {}

// ---------------------------------------------------------------------------------------------------------------------

Fasta::Fasta(genie::util::BitReader& reader, size_t seq_count) : External(reader) {
    for (size_t i = 0; i < seq_count; ++i) {
        seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
        reader.readBypass(seq_checksums.back());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Fasta::Fasta(genie::util::BitReader& reader, uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo,
             size_t seq_count)
    : External(_reserved, std::move(_uri), algo, RefType::FASTA_REF) {
    for (size_t i = 0; i < seq_count; ++i) {
        seq_checksums.emplace_back(checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())], '\0');
        reader.readBypass(seq_checksums.back());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& Fasta::getSeqChecksums() const { return seq_checksums; }

// ---------------------------------------------------------------------------------------------------------------------

void Fasta::addSeqChecksum(std::string checksum) {
    UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                 "Invalid checksum length");
    seq_checksums.emplace_back(std::move(checksum));
}

// ---------------------------------------------------------------------------------------------------------------------

void Fasta::write(genie::util::BitWriter& writer) {
    External::write(writer);
    for (const auto& s : seq_checksums) {
        writer.writeBypass(s.data(), s.length());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Fasta::addChecksum(std::string checksum) {
    UTILS_DIE_IF(checksum.size() != checksum_sizes[static_cast<uint8_t>(getChecksumAlgorithm())],
                 "Invalid checksum length");
    seq_checksums.emplace_back(std::move(checksum));
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace external
}  // namespace location
}  // namespace reference
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
