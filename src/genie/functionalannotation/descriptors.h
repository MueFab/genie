/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FUNCTIONALANNOTATION_DESCRIPTORS_H_
#define SRC_GENIE_FUNCTIONALANNOTATION_DESCRIPTORS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>

#include "genie/functionalannotation/tiles.h"
#include "genie/core/constants.h"
#include "genie/core/record/functional_annotation/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::functional_annotation {

class Descriptors {
 public:
    Descriptors() : maxOntologiesPerRecord(0), rowsPerTile(0), rowInTile(0) { init(); }
    explicit Descriptors(uint64_t _rowsPerTile) : maxOntologiesPerRecord(0), rowsPerTile(_rowsPerTile), rowInTile(0) { init(); }

    void setTileSize(uint64_t _rowsPerTile) {
        rowsPerTile = _rowsPerTile;
        init();
    }

    void write(genie::core::record::functional_annotation::Record functionalAnnotation);

    void setFeatureNameMapping(const std::map<std::string, uint32_t>& mapping) {
        featureNameToIndex = mapping;
    }

    void setOntologyNameMapping(const std::map<std::string, uint32_t>& mapping) {
        ontologyNameToIndex = mapping;
    }

    void setMaxOntologiesPerRecord(uint8_t maxCount) {
        maxOntologiesPerRecord = maxCount;
    }

    std::map<genie::core::AnnotDesc, TiledStream>& getTiles() { return tiles; }

    void writeDanglingBits();

 private:
    std::map<genie::core::AnnotDesc, TiledStream> tiles;
    std::map<std::string, uint32_t> featureNameToIndex;
    std::map<std::string, uint32_t> ontologyNameToIndex;
    uint8_t maxOntologiesPerRecord;

    uint64_t rowsPerTile;
    uint64_t rowInTile;

    void init();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::functional_annotation

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FUNCTIONALANNOTATION_DESCRIPTORS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
