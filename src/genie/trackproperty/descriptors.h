/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_TRACKPROPERTY_DESCRIPTORS_H_
#define SRC_GENIE_TRACKPROPERTY_DESCRIPTORS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/track_property_record/record.h"
#include "genie/trackproperty/tiles.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace track_property {

// ---------------------------------------------------------------------------------------------------------------------

class Descriptors {
 public:
    explicit Descriptors(uint64_t _rowsPerTile) : rowsPerTile(_rowsPerTile) {}
    void write(genie::core::record::track_property::Record trackPropertyRecord);
    void init();
    void writeDanglingBits();
    Tiles& getTiles() { return tiles; }

 private:
    uint64_t rowsPerTile;
    Tiles tiles;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace track_property
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_TRACKPROPERTY_DESCRIPTORS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
