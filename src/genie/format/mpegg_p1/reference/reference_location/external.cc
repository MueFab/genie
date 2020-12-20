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

External::External(std::string&& _ref_uri, ExternalReference&& _ext_ref)
    : ReferenceLocation(ReferenceLocation::Flag::EXTERNAL),
      ref_uri(_ref_uri),
      external_reference(_ext_ref){}

// ---------------------------------------------------------------------------------------------------------------------

External::External(util::BitReader& reader, uint16_t seq_count)
    : ReferenceLocation(ReferenceLocation::Flag::EXTERNAL),
      ref_uri(reader.read<std::string>()),
      checksum_alg(reader.read<Checksum::Algo>()),
      reference_type(reader.read<ExternalReference::Type>()),
      external_reference(){

    switch (reference_type) {
        case ExternalReference::Type::MPEGG_REF: {
            external_reference = MpegReference(reader, checksum_alg);
            break;
        }
        case ExternalReference::Type::RAW_REF:{
            external_reference = RawReference(reader, checksum_alg, seq_count);
            break;
        }
        case ExternalReference::Type::FASTA_REF: {
            external_reference = FastaReference(reader, checksum_alg, seq_count);
            break;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::string External::getRefUri() const { return ref_uri;}

// ---------------------------------------------------------------------------------------------------------------------

const Checksum::Algo External::getChecksumAlg() const { return checksum_alg; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t External::getBitLength() const {

    uint64_t bitlen = (getRefUri().size() + 1)* 8;  //ref_uri st(v)

    //checksum_alg u(8)
    bitlen += 8;
    //reference_type u(8)
    bitlen += 8;

    if (external_reference.getReferenceType() == ExternalReference::Type::MPEGG_REF) {
        bitlen += 8;        // external_dataset_group_ID
        bitlen += 16;       // external_dataset_ID
        if (getChecksumAlg() == Checksum::Algo::MD5) {
            bitlen += 128;      // ref_checksum i(checksum_size)
        }
        else if (getChecksumAlg() == Checksum::Algo::SHA256) {
            bitlen += 256;  // ref_checksum i(checksum_size)
        }
    }
    else {
        for (auto seqID=0; seqID < reference.getSeqCount(); seqID++ {
            if (getChecksumAlg() == Checksum::Algo::MD5) {
                bitlen += 128;      // ref_checksum i(checksum_size)
            }
            else if (getChecksumAlg() == Checksum::Algo::SHA256) {
                bitlen += 256;  // ref_checksum i(checksum_size)
            }
        }
    }

    return bitlen;
};

// ---------------------------------------------------------------------------------------------------------------------

void External::write(util::BitWriter& writer) {
    // ref_uri st(v)
    writer.write(ref_uri);

    // checksum_alg u(8)
    writer.write((uint8_t)checksum_alg, 8);

    // reference_type u(8)
    writer.write((uint8_t)reference_type, 8);

    switch (external_reference.getReferenceType()) {
        case ExternalReference::Type::MPEGG_REF: {
            dynamic_cast<MpegReference*>(&external_reference)->write(writer);
            break;
        }
        case ExternalReference::Type::RAW_REF: {
            dynamic_cast<RawReference*>(&external_reference)->write(writer);
            break;
        }
        case ExternalReference::Type::FASTA_REF: {
            dynamic_cast<FastaReference*>(&external_reference)->write(writer);
            break;
        }
    }
//    // the rest
//    external_reference.write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
