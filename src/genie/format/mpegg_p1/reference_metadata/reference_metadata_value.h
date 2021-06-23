/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_METADATA_REFERENCE_METADATA_VALUE_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_METADATA_REFERENCE_METADATA_VALUE_H_

#include <cstdint>
#include <string>
#include "genie/format/mpegg_p1/reference/reference.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class ReferenceMetadataValue {
    /** ------------------------------------------------------------------------------------------------------------
     *  ISO/IEC 23092-3 Section 6.5.1.4 table 13
     *  ------------------------------------------------------------------------------------------------------------ */
 private:
    uint32_t length;

    //    AlternativeLocusType alternative_locus_location;
    // needs to be fixed _ from part3

    std::string alternative_sequence_name;
    std::string genome_assembly_identifier;
    std::string description;
    std::string species;
    std::string URI;

 public:
    /**
     *
     */
    ReferenceMetadataValue();

    /**
     *
     * @param _length
     * @param _alternative_locus_location
     * @param _alternative_sequence_name
     * @param _genome_assembly_identifier
     * @param _description
     * @param _species
     * @param _URI
     */
    ReferenceMetadataValue(uint32_t _length,
                           //                         AlternativeLocusType _alternative_locus_location,
                           std::string _alternative_sequence_name, std::string _genome_assembly_identifier,
                           std::string _description, std::string _species, std::string&& _URI);

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
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_METADATA_REFERENCE_METADATA_VALUE_H_
