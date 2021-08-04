/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference/reference_location/external_reference/mpegg_ref.h"
#include <utility>
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/md5.h"
#include "genie/format/mpegg_p1/reference/reference_location/external_reference/sha256.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {


// ---------------------------------------------------------------------------------------------------------------------

MpegReference::MpegReference(uint8_t _dataset_group_ID, uint16_t _dataset_ID, std::unique_ptr<Checksum> _ref_checksum)
    : ExternalReference(ExternalReference::Type::MPEGG_REF),
      group_ID(_dataset_group_ID),
      ID(_dataset_ID),
      ref_checksum(nullptr) {

    if (_ref_checksum != nullptr){
        minor_version = "1900";
        ref_checksum = std::move(_ref_checksum);
    } else {
        minor_version = "2000";
    }
}

// ---------------------------------------------------------------------------------------------------------------------

MpegReference::MpegReference(util::BitReader& reader, FileHeader& fhd, Checksum::Algo checksum_alg)
    : ExternalReference(ExternalReference::Type::MPEGG_REF),
      group_ID(reader.read<uint8_t>()),
      ID(reader.read<uint8_t>()),
      ref_checksum(),
      minor_version(fhd.getMinorVersion()){

    if (minor_version == "1900") {
        switch (checksum_alg) {
            case Checksum::Algo::MD5: {
                ref_checksum = genie::util::make_unique<Md5>(reader);
                break;
            }
            case Checksum::Algo::SHA256: {
                ref_checksum = genie::util::make_unique<Sha256>(reader);
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

uint8_t MpegReference::getDatasetGroupID() const { return group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t MpegReference::getDatasetID() const { return ID; }

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Algo MpegReference::getChecksumAlg() const { return ref_checksum->getType(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t MpegReference::getLength() const {

    /// reference_type u(8), group_ID u(16), ID u(16)
    uint64_t len = ExternalReference::getLength() + 1 + 2;

    if (ref_checksum != nullptr){
        switch (ref_checksum->getType()) {
            case Checksum::Algo::MD5: {
                len += dynamic_cast<Md5&>(*ref_checksum).getLength();
                break;
            }
            case Checksum::Algo::SHA256: {
                len += dynamic_cast<Sha256&>(*ref_checksum).getLength();
                break;
            }
            default: {
                UTILS_DIE("Unsupported checksum algorithm");
                break;
            }
        }
    }

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void MpegReference::write(util::BitWriter& writer) const {
    /// reference_type u(8)
    ExternalReference::write(writer);

    writer.write(group_ID, 8);
    writer.write(ID, 16);

    if (ref_checksum != nullptr){
        switch (ref_checksum->getType()) {
            case Checksum::Algo::MD5: {
                dynamic_cast<Md5&>(*ref_checksum).write(writer);
                break;
            }
            case Checksum::Algo::SHA256: {
                dynamic_cast<Sha256&>(*ref_checksum).write(writer);
                break;
            }
            default: {
                UTILS_DIE("Unsupported checksum algorithm");
                break;
            }
        }
    }

    ref_checksum->write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
