/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_VARIANT_SITE_ATTRIBUTES_H_
#define SRC_GENIE_VARIANT_SITE_ATTRIBUTES_H_

// ---------------------------------------------------------------------------------------------------------------------
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/core/record/variant_site/record.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

class AttributeTile {
 public:
    AttributeTile(uint64_t _rowsPerTile, genie::core::record::annotation_parameter_set::AttributeData _info)
        : rowsPerTile(_rowsPerTile), info(_info), tiles{}, rowInTile(0) {}
    AttributeTile() : rowsPerTile(0), info{}, tiles{}, rowInTile(0) {}

    AttributeTile& operator=(const AttributeTile& other);

    AttributeTile(const AttributeTile& other);

    size_t getNrOfTiles() { return tiles.size(); }

    std::stringstream& getTile(uint64_t tilenr) {
        if (tilenr == tiles.size() - 1) {
            writers.back().flush();
       //     convertToTypedData();
        }
        return tiles.at(tilenr);
    }

    void write(std::vector<std::vector<uint8_t>> value);

    void writeMissing();

    void convertToTypedData();

 private:
    uint64_t rowsPerTile;
    genie::core::record::annotation_parameter_set::AttributeData info;
    std::vector<std::stringstream> tiles;
    std::vector<genie::core::Writer> writers;
    uint64_t rowInTile;

    void AddFirst();
};

class Attributes {
 public:
    Attributes() : rowsPerTile(0) {}

    Attributes(uint64_t _rowsPerTile,
               std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData> _info)
        : rowsPerTile(_rowsPerTile), info(_info), attrWritten{} {
        initAttributeTiles();
    }

    void add(std::vector<genie::core::record::variant_site::Info_tag> tags);

    std::map<std::string, AttributeTile>& getTiles() { return attributeTiles; }
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData>& getInfo() { return info; }

    Attributes(Attributes& other);

    Attributes& operator=(const Attributes& other);

 private:
    uint64_t rowsPerTile;
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData> info;
    std::map<std::string, AttributeTile> attributeTiles;
    std::map<std::string, bool> attrWritten;

    void initAttributeTiles();
};

//-------------------------------------------------------------------------------//

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_VARIANT_SITE_ATTRIBUTES_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
