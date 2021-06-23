/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference_metadata/reference_metadata_value.h"
#include <utility>
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadataValue::ReferenceMetadataValue()
    : length(0),
      //      alternative_locus_location(),
      alternative_sequence_name(),
      genome_assembly_identifier(),
      description(),
      species(),
      URI() {}
// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadataValue::ReferenceMetadataValue(
    uint32_t _length,
    //                                               AlternativeLocusType _alternative_locus_location,
    std::string _alternative_sequence_name, std::string _genome_assembly_identifier, std::string _description,
    std::string _species, std::string&& _URI)
    : length(_length),
      //      alternative_locus_location(std::move(_alternative_locus_location)),
      alternative_sequence_name(std::move(_alternative_sequence_name)),
      genome_assembly_identifier(std::move(_genome_assembly_identifier)),
      description(std::move(_description)),
      species(std::move(_species)),
      URI(std::move(_URI))

{}
// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadataValue::ReferenceMetadataValue(util::BitReader& reader, size_t length) {
    size_t start_pos = reader.getPos();

    length = reader.read<uint32_t>();
    //    alternative_locus_location = reader.read<AlternativeLocusType>();
    alternative_sequence_name = readNullTerminatedStr(reader);
    genome_assembly_identifier = readNullTerminatedStr(reader);
    description = readNullTerminatedStr(reader);
    species = readNullTerminatedStr(reader);
    URI = readNullTerminatedStr(reader);

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid DatasetGroup length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ReferenceMetadataValue::getLength() const {
    /// Key c(4) length u(64)
    uint64_t len = (4 * sizeof(char) + 8);  // gen_info

    // length u(32)
    len += 4;

    //    //Class AlternativeLocusType
    //    len += alternative_locus_location.getLength();

    // alternative_sequence_name st(v)
    len += alternative_sequence_name.size() + 1;
    // genome_assembly_identifier st(v)
    len += genome_assembly_identifier.size() + 1;
    // description st(v)
    len += description.size() + 1;
    // species st(v)
    len += species.size() + 1;
    // URI st(v)
    len += URI.size() + 1;

    return len;
}
// ---------------------------------------------------------------------------------------------------------------------

void ReferenceMetadataValue::write(genie::util::BitWriter& bit_writer) const {
    // KLV (Key Length Value) format

    // Key of KLV format
    bit_writer.write("");

    // Length of KLV format
    bit_writer.write(getLength(), 64);

    // length
    bit_writer.write(length, 32);

    //    // alternative_locus_location
    //    alternative_locus_location.write(bit_writer);

    // alternative_sequence_name
    writeNullTerminatedStr(bit_writer, alternative_sequence_name);

    // genome_assembly_identifier
    writeNullTerminatedStr(bit_writer, genome_assembly_identifier);

    // description
    writeNullTerminatedStr(bit_writer, description);

    // species
    writeNullTerminatedStr(bit_writer, species);

    // URI
    writeNullTerminatedStr(bit_writer, URI);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
