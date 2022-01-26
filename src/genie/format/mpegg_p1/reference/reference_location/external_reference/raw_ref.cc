/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference/reference_location/external_reference/raw_ref.h"
#include <memory>
#include <utility>
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/checksum.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/external_reference.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/md5.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/sha256.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

RawReference::RawReference() : ExternalReference(ExternalReference::Type::RAW_REF), checksums() {}

// ---------------------------------------------------------------------------------------------------------------------

RawReference::RawReference(const std::vector<std::unique_ptr<Checksum>> &_checksums) {
    auto checksum = _checksums.begin();
    while (checksum != _checksums.end()) {
        checksums.emplace_back(std::move((*checksum)->clone()));
        checksum++;
    }
}
// ---------------------------------------------------------------------------------------------------------------------

RawReference::RawReference(util::BitReader &reader, FileHeader &, Checksum::Algo checksum_alg, uint16_t seq_count)
    : ExternalReference(ExternalReference::Type::RAW_REF) {
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
                break;
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<ExternalReference> RawReference::clone() const {
    auto ret = util::make_unique<RawReference>(this->checksums);

    return ret;
}
// ---------------------------------------------------------------------------------------------------------------------

Checksum::Algo RawReference::getChecksumAlg() const { return checksums.front()->getType(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t RawReference::getLength() const {
    uint64_t len = 0;
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

void RawReference::write(util::BitWriter &writer) const {
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
