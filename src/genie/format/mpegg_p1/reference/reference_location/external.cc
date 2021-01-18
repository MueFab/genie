#include <genie/util/make-unique.h>

#include "external.h"
#include "external_reference/checksum.h"
#include "external_reference/external_reference.h"
#include "external_reference/mpegg_ref.h"
#include "external_reference/raw_ref.h"
#include "external_reference/fasta_ref.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

//External::External(std::string&& _ref_uri, ExternalReference&& _ext_ref)
//    : ReferenceLocation(ReferenceLocation::Flag::EXTERNAL),
//      ref_uri(_ref_uri),
//      external_reference(_ext_ref){}

// ---------------------------------------------------------------------------------------------------------------------

External::External(util::BitReader& reader, uint16_t seq_count)
    : ReferenceLocation(ReferenceLocation::Flag::EXTERNAL),
      ref_uri(reader.read<std::string>()) {

    auto checksum_alg = reader.read<Checksum::Algo>();
    auto reference_type = reader.read<ExternalReference::Type>();

    if (reference_type == ExternalReference::Type::MPEGG_REF){
        external_reference = util::make_unique<MpegReference>(reader, checksum_alg);
    } else if (reference_type == ExternalReference::Type::RAW_REF){
        external_reference = util::make_unique<RawReference>(reader, checksum_alg, seq_count);
    } else if (reference_type == ExternalReference::Type::FASTA_REF){
        external_reference = util::make_unique<FastaReference>(reader, checksum_alg, seq_count);
    }

}

// ---------------------------------------------------------------------------------------------------------------------

std::string External::getRefUri() const { return ref_uri;}

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Algo External::getChecksumAlg() const { return external_reference->getChecksumAlg(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t External::getLength() const {


//    UTILS_DIE("Not yet implemented");
    //ref_uri st(v)
    uint64_t len = (getRefUri().size() + 1);
    //checksum_alg u(8)
    len += 1;
    //reference_type u(8)
    len += 1;

    len += external_reference->getlength();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void External::write(util::BitWriter& writer) const {
    // ref_uri st(v)
    writer.write(ref_uri);

    // checksum_alg u(8)
    writer.write((uint8_t) getChecksumAlg(), 8);

    // reference_type u(8)
    writer.write((uint8_t) external_reference->getReferenceType(), 8);

    // external_datset_group_ID, external_dataset_ID, ref_checksum or checksum[seqID]
    external_reference->write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
