/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <memory>
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/raw_ref.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/checksum.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/external_reference.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/md5.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/sha256.h"
#include "genie/util/make-unique.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

RawReference::RawReference() : ExternalReference(ExternalReference::Type::RAW_REF) {}

// ---------------------------------------------------------------------------------------------------------------------

RawReference::RawReference(util::BitReader &reader, FileHeader& fhd, Checksum::Algo checksum_alg, uint16_t seq_count) {
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

//RawReference::RawReference(std::vector<std::unique_ptr<Checksum>> &&_checksums)
//    : ExternalReference(ExternalReference::Type::RAW_REF), checksums(_checksums) {
////    for (auto &checksum: checksums) {
////        UTILS_DIE_IF(checksums.front().getType() != checksum.getType(), "Different checksum algorithm");
////    }
//}

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Algo RawReference::getChecksumAlg() const { return checksums.front()->getType(); }

// ---------------------------------------------------------------------------------------------------------------------

void RawReference::write(util::BitWriter &bitwriter) const {
    for (auto &checksum : checksums) {
        checksum->write(bitwriter);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
