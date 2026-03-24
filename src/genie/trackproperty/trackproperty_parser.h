/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_TRACKPROPERTY_TRACKPROPERTY_PARSER_H_
#define SRC_GENIE_TRACKPROPERTY_TRACKPROPERTY_PARSER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <string>
#include <vector>

#include "genie/annotation/json_attribute_parser.h"
#include "genie/core/track_property_record/record.h"
#include "genie/trackproperty/descriptors.h"
#include "genie/util/bit_reader.h"
#include "genie/util/runtime_exception.h"
#include "genie/variantsite/attributes.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace track_property {

// ---------------------------------------------------------------------------------------------------------------------

class TrackPropertyParser {
 public:
    TrackPropertyParser(std::ifstream& inputfile, std::vector<annotation::InfoField> infofields,
                        uint64_t _defaultTileSizeHeight);

    size_t getNumberOfRows() const { return numberOfRows; }
    Descriptors& getDescriptors() { return descriptors; }
    variant_site::Attributes& getAttributes() { return attributes; }
    uint64_t getNrOfTiles() { return numberOfTiles; }

 private:
    core::record::track_property::Record trackPropertyRecord;
    std::istream& trackPropertyMGrecs;
    uint64_t rowsPerTile;
    size_t numberOfRows;
    std::vector<annotation::InfoField> infoFields;
    
    Descriptors descriptors;
    variant_site::Attributes attributes;
    uint64_t numberOfTiles{0};

    void init();
    bool fillRecord(util::BitReader reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace track_property
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_TRACKPROPERTY_TRACKPROPERTY_PARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
