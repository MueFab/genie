/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_PART1_REFERENCE_METADATA_VALUE_H
#define GENIE_PART1_REFERENCE_METADATA_VALUE_H

#include <cstdint>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/format/mpegg_p1/reference/reference.h>

namespace genie {
namespace format {
namespace mpegg_p1 {


class ReferenceMetadataValue{
    /** ------------------------------------------------------------------------------------------------------------
    *  ISO/IEC 23092-3 Section 6.5.1.4 table 13
    *  ------------------------------------------------------------------------------------------------------------ */
   private:

    uint32_t length;
    AlternativeLocusType alternative_locus_location;
//TODO: create class AlternativeLocusType()
    //std::string chromosome_name;
    //position;
    //long start;
    //long end;
    std::string alternative_sequence_name;
    std::string genome_assembly_identifier;
    std::string description;
    std::string species;
    anyURI URI;

   public:
    /**
    *
    * default constructor
    */
    ReferenceMetadataValue();

    /**
    *
    * parameter
    */
    ReferenceMetadataValue(long _length, AlternativeLocusType _alternative_locus_location,
                           std::string _alternative_sequence_name, std::string _genome_assembly_identifier,
                           std::string _description, std::string _species, anyURI _URI);

    /**
     *
     * @param reader
     * @param length
     */
    ReferenceMetadataValue(util::BitReader& reader, size_t length);

    /**
    *
    * @return
    */
    uint64_t getLength() const;

    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_REFERENCE_METADATA_VALUE_H
