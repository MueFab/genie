/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference/reference_location/external_reference/fasta_ref.h"
#include <utility>
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/md5.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/sha256.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

FastaReference::FastaReference() : ExternalReference(ExternalReference::Type::FASTA_REF), checksums() {}

// ---------------------------------------------------------------------------------------------------------------------

FastaReference::FastaReference(const std::vector<std::unique_ptr<Checksum>> &_checksums) {
    auto checksum = _checksums.begin();
    while (checksum != _checksums.end()) {
        checksums.emplace_back(std::move((*checksum)->clone()));
        checksum++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

FastaReference::FastaReference(util::BitReader &reader, FileHeader &, Checksum::Algo checksum_alg, uint16_t seq_count)
    : ExternalReference(ExternalReference::Type::FASTA_REF) {
    for (auto i = 0; i < seq_count; i++) {
        switch (checksum_alg) {
            case Checksum::Algo::MD5: {
                auto checksum = genie::util::make_unique<Md5>(reader);
                checksums.push_back(std::move(checksum));
                break;
            }
            case Checksum::Algo::SHA256: {
                auto checksum = genie::util::make_unique<Sha256>(reader);
                checksums.push_back(std::move(checksum));
                break;
            }
            default: {
                UTILS_DIE("Unsupported checksum algorithm");
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<ExternalReference> FastaReference::clone() const {
    auto ret = util::make_unique<FastaReference>(this->checksums);

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Algo FastaReference::getChecksumAlg() const { return checksums.front()->getType(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t FastaReference::getLength() const {
    uint64_t len = ExternalReference::getLength();

    /// Only write if the content is not empty as to get the checksum_alg requires at least 1 entry
    if (!checksums.empty()) {
        auto checksum_algo = getChecksumAlg();
        for (auto &checksum : checksums) {
            switch (checksum_algo) {
                case Checksum::Algo::MD5: {
                    len += dynamic_cast<Md5 &>(*checksum).getLength();
                    break;
                }
                case Checksum::Algo::SHA256: {
                    len += dynamic_cast<Sha256 &>(*checksum).getLength();
                    break;
                }
                default: {
                    UTILS_DIE("Unsupported checksum algorithm");
                    break;
                }
            }
        }
    }
    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void FastaReference::write(util::BitWriter &writer) const {
    ExternalReference::write(writer);

    /// Only write if the content is not empty as to get the checksum_alg requires at least 1 entry
    if (!checksums.empty()) {
        auto checksum_algo = getChecksumAlg();
        for (auto &checksum : checksums) {
            switch (checksum_algo) {
                case Checksum::Algo::MD5: {
                    dynamic_cast<Md5 &>(*checksum).write(writer);
                    break;
                }
                case Checksum::Algo::SHA256: {
                    dynamic_cast<Sha256 &>(*checksum).write(writer);
                    break;
                }
                default: {
                    UTILS_DIE("Unsupported checksum algorithm");
                    break;
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
