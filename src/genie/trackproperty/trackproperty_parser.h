/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_TRACKPROPERTY_TRACKPROPERTY_PARSER_H_
#define SRC_GENIE_TRACKPROPERTY_TRACKPROPERTY_PARSER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "genie/variantsite/attributes.h"
#include "genie/trackproperty/descriptors.h"
#include "genie/annotation/json_attribute_parser.h"
#include "genie/core/parameter/annotation/attribute_data.h"
#include "genie/core/record/track_property/record.h"
#include "genie/util/bit_reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::track_property {

// ---------------------------------------------------------------------------------------------------------------------

class TrackPropertyParser {
 public:
    using AttributeData = core::parameter::annotation::AttributeData;

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

    std::map<std::string, AttributeData> attributeData;
    Descriptors descriptors;
    variant_site::Attributes attributes;
    uint16_t numberOfAttributes;
    uint64_t numberOfTiles{0};

    void init();
    bool fillRecord(util::BitReader reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::track_property

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_TRACKPROPERTY_TRACKPROPERTY_PARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
