/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reference_location.h"
#include <memory>
#include "genie/util/bitreader.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

std::unique_ptr<ReferenceLocation> ReferenceLocation::referenceLocationFactory(genie::util::BitReader& reader,
                                                                               size_t seq_count,
                                                                               genie::core::MPEGMinorVersion _version) {
    auto _reserved = reader.read<uint8_t>(7);
    bool _external_ref_flag = reader.read<bool>(1);
    if (!_external_ref_flag) {
        return genie::util::make_unique<InternalReferenceLocation>(reader, _reserved);
    } else {
        return ExternalReferenceLocation::externalReferenceLocationFactory(reader, _reserved, seq_count, _version);
    }
}

std::unique_ptr<ExternalReferenceLocation> ExternalReferenceLocation::externalReferenceLocationFactory(
    genie::util::BitReader& reader, uint8_t _reserved, size_t seq_count, genie::core::MPEGMinorVersion _version) {
    std::string _ref_uri;
    reader.readBypass_null_terminated(_ref_uri);
    auto _checksum_algo = reader.readBypassBE<ChecksumAlgorithm>();
    auto _ref_type = reader.readBypassBE<RefType>();
    switch (_ref_type) {
        case RefType::MPEGG_REF:
            return genie::util::make_unique<ExternalReferenceLocationMPEGG>(reader, _reserved, std::move(_ref_uri),
                                                                            _checksum_algo, seq_count, _version);
        case RefType::RAW_REF:
            return genie::util::make_unique<ExternalReferenceLocationRaw>(reader, _reserved, std::move(_ref_uri),
                                                                          _checksum_algo, seq_count);
        case RefType::FASTA_REF:
            return genie::util::make_unique<ExternalReferenceLocationFasta>(reader, _reserved, std::move(_ref_uri),
                                                                            _checksum_algo, seq_count);
        default:
            UTILS_DIE("Unknown ref type");
    }
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
