/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference/reference_location/external.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

External::External(util::BitReader& bitreader, FileHeader& fhd, uint16_t seq_count)
    : ReferenceLocation(ReferenceLocation::Flag::EXTERNAL),
      ref_uri(readNullTerminatedStr(bitreader)) {

    auto checksum_alg = bitreader.read<Checksum::Algo>();             // checksum_alg u(8)
    auto reference_type = bitreader.read<ExternalReference::Type>();  // reference_type u(8)

    if (reference_type == ExternalReference::Type::MPEGG_REF) {
        external_reference = util::make_unique<MpegReference>(bitreader, fhd, checksum_alg);
    } else if (reference_type == ExternalReference::Type::RAW_REF) {
        external_reference = util::make_unique<RawReference>(bitreader, fhd, checksum_alg, seq_count);
    } else if (reference_type == ExternalReference::Type::FASTA_REF) {
        external_reference = util::make_unique<FastaReference>(bitreader, fhd, checksum_alg, seq_count);
    } else {
        UTILS_DIE("Unsupported reference_type");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::string External::getRefUri() const { return ref_uri; }

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Algo External::getChecksumAlg() const {
    return external_reference->getChecksumAlg();
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t External::getLength() const {
    // reserved u(7), external_ref_flag u(1)
    uint64_t len = ReferenceLocation::getLength();

    // ref_uri st(v)
    len += (getRefUri().size() + 1);
    // checksum_alg u(8)
    len += 1;
    // reference_type u(8)
    len += 1;

    len += external_reference->getLength();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void External::write(util::BitWriter& bitwriter) const {
    // reserved u(7), external_ref_flag u(1)
    ReferenceLocation::write(bitwriter);

    // ref_uri st(v)
    writeNullTerminatedStr(bitwriter, ref_uri);

    // checksum_alg u(8)
    bitwriter.write((uint8_t)getChecksumAlg(), 8);

    // reference_type u(8)
    bitwriter.write((uint8_t)external_reference->getReferenceType(), 8);

    // external_dataset_group_ID, external_dataset_ID, ref_checksum or checksum[seqID]
    external_reference->write(bitwriter);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
