/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference/reference_location/external.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

External::External(): ReferenceLocation(ReferenceLocation::Flag::EXTERNAL),
                       ref_uri(), checksum_alg(Checksum::Algo::UNKNOWN), external_reference(nullptr){}

// ---------------------------------------------------------------------------------------------------------------------

External::External(const External& other){*this = other; }

// ---------------------------------------------------------------------------------------------------------------------

External::External(std::string& _ref_uri, Checksum::Algo _checksum_alg, std::unique_ptr<ExternalReference> _ext_ref)
    : ReferenceLocation(ReferenceLocation::Flag::EXTERNAL),
      ref_uri(_ref_uri),
      checksum_alg(_checksum_alg),
      external_reference(std::move(_ext_ref)){}

// ---------------------------------------------------------------------------------------------------------------------

External::External(util::BitReader& reader, FileHeader& fhd, uint16_t seq_count)
    : ReferenceLocation(ReferenceLocation::Flag::EXTERNAL){

    ref_uri = readNullTerminatedStr(reader); /// ref_uri st(v)
    checksum_alg = reader.read<Checksum::Algo>(); /// checksum_alg u(8)

    auto startpos = reader.getPos();
    auto reference_type = reader.read<ExternalReference::Type>(); /// reference_type u(8)
    if (reference_type == ExternalReference::Type::MPEGG_REF) {
        external_reference = util::make_unique<MpegReference>(reader, fhd, checksum_alg);
        auto writesize = dynamic_cast<MpegReference&>(*external_reference).getLength();
        auto readsize = reader.getPos() - startpos;
        UTILS_DIE_IF(writesize != readsize, "MpegReference has size");
    } else if (reference_type == ExternalReference::Type::RAW_REF) {
        external_reference = util::make_unique<RawReference>(reader, fhd, checksum_alg, seq_count);
        auto writesize = dynamic_cast<RawReference&>(*external_reference).getLength();
        auto readsize = reader.getPos() - startpos;
        UTILS_DIE_IF(writesize != readsize, "RawReference has different size");
    } else if (reference_type == ExternalReference::Type::FASTA_REF) {
        external_reference = util::make_unique<FastaReference>(reader, fhd, checksum_alg, seq_count);
        auto writesize = dynamic_cast<FastaReference&>(*external_reference).getLength();
        auto readsize = reader.getPos() - startpos;
        UTILS_DIE_IF(writesize != readsize, "FastaReference has different size");
    } else {
        UTILS_DIE("Unsupported reference_type");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

External& External::operator=(const External& other) {
//    if (this == &container) {
//        return *this;
//    }
    this->ref_uri = other.ref_uri;
    this->checksum_alg = other.checksum_alg;

    this->external_reference = other.external_reference->clone();

    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string External::getRefUri() const { return ref_uri; }

// ---------------------------------------------------------------------------------------------------------------------

Checksum::Algo External::getChecksumAlg() const {
    return external_reference->getChecksumAlg();
}

ExternalReference& External::getExternalRef() const{
    return *(external_reference.get());
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t External::getLength() const {
    /// reserved u(7), external_ref_flag u(1)
    uint64_t len = ReferenceLocation::getLength();

    len += (getRefUri().size() + 1); /// ref_uri st(v)

    len += 1; /// checksum_alg u(8)

    switch (external_reference->getType()){
        case (ExternalReference::Type::MPEGG_REF):
            len += dynamic_cast<MpegReference&>(*external_reference).getLength();
            break;
        case (ExternalReference::Type::FASTA_REF):
            len += dynamic_cast<FastaReference&>(*external_reference).getLength();
            break;
        case (ExternalReference::Type::RAW_REF):
            len += dynamic_cast<RawReference&>(*external_reference).getLength();
            break;
        default:
            UTILS_DIE("Invalid external reference type!");
            break;
    }

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void External::write(util::BitWriter& writer) const {
    /// reserved u(7), external_ref_flag u(1)
    ReferenceLocation::write(writer);

    /// ref_uri st(v)
    writeNullTerminatedStr(writer, ref_uri);

    /// checksum_alg u(8)
    writer.write((uint8_t)checksum_alg, 8);

    /// reference_type u(8), external_dataset_group_ID, external_dataset_ID, ref_checksum or checksum[seqID]
    switch (external_reference->getType()){
        case (ExternalReference::Type::MPEGG_REF):
            dynamic_cast<MpegReference&>(*external_reference).write(writer);
            break;
        case (ExternalReference::Type::FASTA_REF):
            dynamic_cast<FastaReference&>(*external_reference).write(writer);
            break;
        case (ExternalReference::Type::RAW_REF):
            dynamic_cast<RawReference&>(*external_reference).write(writer);
            break;
        default:
            UTILS_DIE("Invalid external reference type!");
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
