#include <genie/util/exception.h>
#include <genie/util/runtime-exception.h>

#include "reference_metadata_value.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadataValue::ReferenceMetadataValue()
    : length(0),
      alternative_locus_location(),
      alternative_sequence_name(),
      genome_assembly_identifier(),
      description(),
      species(),
      URI()
{}
// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadataValue::ReferenceMetadataValue(long _length, AlternativeLocusType _alternative_locus_location,
                                               std::string _alternative_sequence_name, std::string _genome_assembly_identifier,
                                               std::string _description, std::string _species, anyURI _URI)
    : length(_length),
      alternative_locus_location(std::move(_alternative_locus_location)),
      alternative_sequence_name(std::move(_alternative_sequence_name)),
      genome_assembly_identifier(std::move(_genome_assembly_identifier)),
      description(std::move(_description)),
      species(std::move(_species)),
      URI(std::move(_URI))

{}
// ---------------------------------------------------------------------------------------------------------------------

ReferenceMetadataValue(util::BitReader& reader, size_t length)
    : length(0),
      alternative_locus_location(),
      alternative_sequence_name(),
      genome_assembly_identifier(),
      description(),
      species(),
      URI() {

    size_t start_pos = reader.getPos();

    length = reader.read<uint32_t>();
    alternative_locus_location = reader.read<AlternativeLocusType>();
    alternative_sequence_name = reader.read<std::string>();
    genome_assembly_identifier = reader.read<std::string>();
    description = reader.read<std::string>();
    species = reader.read<std::string>();
    URI = reader.read<anyURI>();

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid DatasetGroup length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t getLength() const {
/*
    //Key c(4) length u(64)
    uint64_t len = (4* sizeof(char) + 8) * 8;   //gen_info

    //length u(32)
    len += 32;

//TODO : length of Value[]

    return len;
*/
}
// ---------------------------------------------------------------------------------------------------------------------

void write(genie::util::BitWriter& bit_writer) const {
    /*
    // KLV (Key Length Value) format

    // Key of KLV format
    bit_writer.write("");

    // Length of KLV format
    bit_writer.write(getLength(), 64);

    // length
    bit_writer.write(length, 8);

    // alternative_locus_location
    alternative_locus_location.write(bit_writer);

    //alternative_sequence_name
    bit_writer.write(alternative_sequence_name);

    //genome_assembly_identifier
    bitwriter.write(genome_assembly_name);

    //description
    bitwriter.write(description);

    //species
    bit_writer.write(species);

    //URI
    bit_writer.write(URI);
*/
}


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
